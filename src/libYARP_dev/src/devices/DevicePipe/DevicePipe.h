/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_DEVICEPIPE_H
#define YARP_DEV_DEVICEPIPE_H

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ServiceInterfaces.h>


namespace yarp{
    namespace dev {
        class DevicePipe;
    }
}

/**
 * @ingroup dev_impl_other
 *
 * Tries to connect the output of one device to the input of another.
 *
 */
class YARP_dev_API yarp::dev::DevicePipe : public DeviceDriver,
                                           public IService {

public:
    bool open(yarp::os::Searchable& config) override;

    bool close() override;

    bool startService() override {
        // please call updateService
        return false;
    }

    bool stopService() override {
        return close();
    }

    bool updateService() override;

protected:
    PolyDriver source, sink;

    bool open(const char *key, PolyDriver& poly,
              yarp::os::Searchable& config, const char *comment);
};


#endif // YARP_DEV_DEVICEPIPE_H
