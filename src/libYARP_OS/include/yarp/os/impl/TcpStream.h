/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_TCPSTREAM_H
#define YARP_OS_IMPL_TCPSTREAM_H

#if defined(YARP_HAS_ACE)
# include <ace/config.h>
# include <ace/SOCK_Stream.h>
// In one the ACE headers there is a definition of "main" for WIN32
# ifdef main
#  undef main
# endif
#elif defined(__unix__)
# include <yarp/os/impl/posix/TcpStream.h>
#else
YARP_COMPILER_ERROR(Cannot implement TcpStream on this platform)
#endif

namespace yarp {
namespace os {
namespace impl {

#ifdef YARP_HAS_ACE
typedef ACE_SOCK_Stream TcpStream;
#elif defined(__unix__)
typedef yarp::os::impl::posix::TcpStream TcpStream;
#endif

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_TCPSTREAM_H
