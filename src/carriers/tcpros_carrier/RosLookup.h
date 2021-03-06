/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <tcpros_carrier_api.h>

#include <yarp/os/Contact.h>

#include <cstdio>
#include <string>

class YARP_tcpros_carrier_API RosLookup {
public:
    bool valid;
    std::string hostname;
    int portnum;
    std::string protocol;
    bool verbose;

    RosLookup(bool verbose) :
        valid(false),
        portnum(-1),
        verbose(verbose)
    {}

    bool lookupCore(const std::string& name);

    bool lookupTopic(const std::string& name);

    std::string toString() const {
        char buf[1000];
        sprintf(buf,"/%s:%d/", hostname.c_str(), portnum);
        return buf;
    }

    yarp::os::Contact toContact(const char *carrier) {
        return yarp::os::Contact(carrier,hostname.c_str(), portnum);
    }

    static yarp::os::Contact getRosCoreAddressFromEnv();
    static yarp::os::Contact getRosCoreAddress();
};
