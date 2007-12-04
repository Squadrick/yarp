
// based on cvcap_vfw.cpp from the OpenCV library.
// changes under the GPL. (c) 2007 Paul Fitzpatrick.


/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/



#include <stdio.h>

#include <vfw.h>
#include <wvfw.h>
#include <ovfw.h>
#include <mvfw.h>

#define True 1

enum {
  CV_CAP_PROP_FRAME_HEIGHT,
  CV_CAP_PROP_FRAME_WIDTH,
  CV_CAP_PROP_FOURCC,
};

/*
class IplImage {
public:
  int width, height;
  void *imageData;
};
*/


#include <yarp/sig/Image.h>
#include <yarp/sig/ImageFile.h>

typedef yarp::sig::ImageOf<yarp::sig::PixelBgr> Image;


typedef struct CvCaptureCAM_VFW
{
  CAPDRIVERCAPS caps;
  HWND   capWnd;
  VIDEOHDR* hdr;
  DWORD  fourcc;
  HIC    hic;
  Image frame;
}
CvCaptureCAM_VFW;


static LRESULT PASCAL FrameCallbackProc( HWND hWnd, VIDEOHDR* hdr ) 
{ 
    CvCaptureCAM_VFW* capture = 0;

    if (!hWnd) return FALSE;

    capture = (CvCaptureCAM_VFW*)capGetUserData(hWnd);
    capture->hdr = hdr;

    return (LRESULT)TRUE; 
} 


// Initialize camera input
static int icvOpenCAM_VFW( CvCaptureCAM_VFW* capture, int wIndex )
{
    char szDeviceName[80];
    char szDeviceVersion[80];
    HWND hWndC = 0;
    
    if( (unsigned)wIndex >= 10 )
        wIndex = 0;

    for( ; wIndex < 10; wIndex++ ) 
    {
        if( capGetDriverDescription( wIndex, szDeviceName, 
            sizeof (szDeviceName), szDeviceVersion, 
            sizeof (szDeviceVersion))) 
        {
	  printf("Got something (%s)\n", szDeviceName);
            hWndC = capCreateCaptureWindow ( "My Own Capture Window", 
                WS_POPUP | WS_CHILD, 0, 0, 320, 240, 0, 0);
            if( capDriverConnect (hWndC, wIndex))
                break;
            DestroyWindow( hWndC );
            hWndC = 0;
        }
    }
    
    capture->capWnd = 0;
    if( hWndC )
    {
      printf("got a window\n");
        capture->capWnd = hWndC;
        capture->hdr = 0;
        capture->hic = 0;
        capture->fourcc = (DWORD)-1;
        
        memset( &capture->caps, 0, sizeof(capture->caps));
        capDriverGetCaps( hWndC, &capture->caps, sizeof(&capture->caps));
        ::MoveWindow( hWndC, 0, 0, 320, 240, TRUE );
        capSetUserData( hWndC, (size_t)capture );
        capSetCallbackOnFrame( hWndC, FrameCallbackProc ); 
        CAPTUREPARMS p;
        capCaptureGetSetup(hWndC,&p,sizeof(CAPTUREPARMS));
        p.dwRequestMicroSecPerFrame = 66667/2;
        capCaptureSetSetup(hWndC,&p,sizeof(CAPTUREPARMS));
        //capPreview( hWndC, 1 );
        capPreviewScale(hWndC,FALSE);
        capPreviewRate(hWndC,1);
    }
    return capture->capWnd != 0;
}

static  void icvCloseCAM_VFW( CvCaptureCAM_VFW* capture )
{
    if( capture && capture->capWnd )
    {
        capSetCallbackOnFrame( capture->capWnd, NULL ); 
        capDriverDisconnect( capture->capWnd );
        DestroyWindow( capture->capWnd );
        if( capture->hic )
        {
            ICDecompressEnd( capture->hic );
            ICClose( capture->hic );
        }

        capture->capWnd = 0;
        capture->hic = 0;
        capture->hdr = 0;
        capture->fourcc = 0;
        //memset( &capture->frame, 0, sizeof(capture->frame));
    }
}


static int icvGrabFrameCAM_VFW( CvCaptureCAM_VFW* capture )
{
    if( capture->capWnd )
    {
        SendMessage( capture->capWnd, WM_CAP_GRAB_FRAME_NOSTOP, 0, 0 );
        return 1;
    }
    return 0;
}


static BITMAPINFOHEADER icvBitmapHeader( int width, int height, int bpp, int compression = BI_RGB )
{
    BITMAPINFOHEADER bmih;
    memset( &bmih, 0, sizeof(bmih));
    bmih.biSize = sizeof(bmih);
    bmih.biWidth = width;
    bmih.biHeight = height;
    bmih.biBitCount = (WORD)bpp;
    bmih.biCompression = compression;
    bmih.biPlanes = 1;

    return bmih;
}

static Image* icvRetrieveFrameCAM_VFW( CvCaptureCAM_VFW* capture )
{
  bool done = false;
    if( capture->capWnd )
    {
        BITMAPINFO vfmt;
        memset( &vfmt, 0, sizeof(vfmt));
        int sz = capGetVideoFormat( capture->capWnd, &vfmt, sizeof(vfmt));

        if( capture->hdr && capture->hdr->lpData && sz != 0 )
        {
            long code = ICERR_OK;
            char* frame_data = (char*)capture->hdr->lpData;

            if( vfmt.bmiHeader.biCompression != BI_RGB ||
                vfmt.bmiHeader.biBitCount != 24 )
            {
	      printf("funky format\n");
                BITMAPINFOHEADER& vfmt0 = vfmt.bmiHeader;
                BITMAPINFOHEADER vfmt1 = icvBitmapHeader( vfmt0.biWidth, vfmt0.biHeight, 24 );
                code = ICERR_ERROR;

                if( capture->hic == 0 ||
                    capture->fourcc != vfmt0.biCompression ||
                    vfmt0.biWidth != capture->frame.width() ||
                    vfmt0.biHeight != capture->frame.height() )
                {
                    if( capture->hic )
                    {
                        ICDecompressEnd( capture->hic );
                        ICClose( capture->hic );
                    }
                    capture->hic = ICOpen( MAKEFOURCC('V','I','D','C'),
                                            vfmt0.biCompression, ICMODE_DECOMPRESS );
                    if( capture->hic &&
                        ICDecompressBegin( capture->hic, &vfmt0, &vfmt1 ) == ICERR_OK )
                    {

                        //capture->rgb_frame = cvCreateImage(
			//  cvSize( vfmt0.biWidth, vfmt0.biHeight ), IPL_DEPTH_8U, 3 );

			capture->frame.resize(vfmt0.biWidth, vfmt0.biHeight);
                        capture->frame.setTopIsLowIndex(false);

                        code = ICDecompress( capture->hic, 0,
                                             &vfmt0, capture->hdr->lpData,
                                             &vfmt1, capture->frame.getRawImage() );
			if (code == ICERR_OK) {
			  done = true;
			}

                    }
                }
            }
        
            if( code == ICERR_OK )
            {
	      printf("GOT IMAGE\n");
	      /*
	      cvInitImageHeader( &capture->frame,
				 cvSize(vfmt.bmiHeader.biWidth,
					vfmt.bmiHeader.biHeight),
				 IPL_DEPTH_8U, 3, IPL_ORIGIN_BL, 4 );
	      capture->frame.imageData = capture->frame.imageDataOrigin = frame_data;
	      */
	      if (!done) {
		capture->frame.setExternal(frame_data,
					   vfmt.bmiHeader.biWidth,
					   vfmt.bmiHeader.biHeight);
	      }
	      return &capture->frame;
            }
        }
    }

    return 0;
}


static double icvGetPropertyCAM_VFW( CvCaptureCAM_VFW* capture, int property_id )
{
    switch( property_id )
    {
    case CV_CAP_PROP_FRAME_WIDTH:
        return capture->frame.width();
    case CV_CAP_PROP_FRAME_HEIGHT:
        return capture->frame.height();
    case CV_CAP_PROP_FOURCC:
        return capture->fourcc;
    }
    return 0;
}



int main() {
  CvCaptureCAM_VFW state;
  int result = icvOpenCAM_VFW(&state,0);
  if (!result) {
    printf("failed to find camera\n");
    return 1;
  }
  for (int i=0; i<10; i++) {
    icvGrabFrameCAM_VFW(&state);
    Image *img = icvRetrieveFrameCAM_VFW(&state);
    printf("image size %d %d\n", img->width(), img->height());
    char fname[256];
    sprintf(fname,"img%06d.ppm",i);
    yarp::sig::file::write(*img,fname);
  }
  icvCloseCAM_VFW(&state);
  return 0;
}
