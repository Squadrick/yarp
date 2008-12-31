#ifndef __VIEWERRESOURCES__
#define __VIEWERRESOURCES__

#include <gtk/gtk.h>
#include "YarpImage2Pixbuf.h"

#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>

#include "FpsStats.h"

class ViewerResources
{
public:
    ViewerResources()
    {
        frame=0;
        drawArea=0;
        fpsData=0;
        scaledFrame=0;
        freezed=false;
        windowH=0;
        windowW=0;
    }

    ~ViewerResources()
    {
        if (frame)
            g_object_unref(frame);

        if (scaledFrame)
            g_object_unref(scaledFrame);
    }

    bool attach(FpsStats *d)
    {
        if (fpsData!=0)
            return false;
        fpsData=d;
        return true;
    }

    void freeze()
    {
        freezed=true;
    }

    void unfreeze()
    {
        freezed=false;
    }

    bool isFreezed()
    { return freezed; }

    void lock() { mutex.wait(); }
    void unlock() { mutex.post(); }

    void invalidateDrawArea()
    {
        if (drawArea)
            gtk_widget_queue_draw (drawArea);
    }

    void draw(GtkWidget *widget, 
        unsigned int areaX,
        unsigned int areaY,
        unsigned int areaW,
        unsigned int areaH,
        bool force=false
        )
    {
        guchar *pixels;
        unsigned int rowstride;

        lock();
        if ( (!force) && freezed)
        {
            unlock();
            return;
        }

        unsigned int imageH=yimage.height();
        unsigned int imageW=yimage.width();

        if ((imageH==0) || (imageW==0))
        {
            //nothing to draw
            unlock();
            return;
        }

        if (fpsData!=0)
            fpsData->update(yarp::os::Time::now());

        if (!frame)
            frame=gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, imageH, imageW);

        //first rescale frame depending on image size
        unsigned int frameH=gdk_pixbuf_get_height(frame);
        unsigned int frameW=gdk_pixbuf_get_width(frame);

        if ( (frameH!=imageH) || (frameW!=imageW) || !frame)
        {
            if (frame)
                g_object_unref(frame);

            frame=gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, imageH, imageW);
        }

        //copy to frame
        yarpImage2Pixbuf(&yimage, frame);

        if (scaledFrame)
            g_object_unref(scaledFrame);

        scaledFrame = gdk_pixbuf_scale_simple(frame,
            windowW,
            windowH,
            GDK_INTERP_BILINEAR); // Best quality

        pixels = gdk_pixbuf_get_pixels (scaledFrame);
        rowstride = gdk_pixbuf_get_rowstride(scaledFrame);
        gdk_draw_rgb_image (widget->window,
            widget->style->black_gc,
            areaX, areaY,
            windowW, windowH,
            GDK_RGB_DITHER_NORMAL,
            pixels,
            rowstride);
        unlock();
    }

    void pushImage(yarp::sig::FlexImage &n)
    {
        if (freezed)
            return;

        lock();
        yimage.copy(n);
        unlock();
    }

    void configure(GtkWidget *widget,
        unsigned int width,
        unsigned int height)
    {
        lock();
        windowW=width;
        windowH=height;
        unlock();
    }

    unsigned int imageHeight()
    {
        lock();
        unsigned int ret=yimage.height();
        unlock();

        return ret;
    }

    unsigned int imageWidth()
    {
        lock();
        unsigned int ret=yimage.width();
        unlock();
        return ret;
    }

    GtkWidget *drawArea;

private:
    GdkPixbuf *frame;
    GdkPixbuf *scaledFrame;
 
    FpsStats  *fpsData;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> yimage;

    bool freezed;

    unsigned int windowW;
    unsigned int windowH;

    yarp::os::Semaphore mutex;
};


#endif
