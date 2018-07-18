/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_Demo
#define YARP_THRIFT_GENERATOR_Demo

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <secondInterface/demo_common.h>
#include <firstInterface/PointD.h>

namespace yarp {
  namespace test {
    class Demo;
  }
}


class yarp::test::Demo : public yarp::os::Wire {
public:
  Demo();
  virtual std::int32_t get_answer();
  virtual std::int32_t add_one(const std::int32_t x = 0);
  virtual std::int32_t double_down(const std::int32_t x);
  virtual  ::yarp::test::PointD add_point(const  ::yarp::test::PointD& x, const  ::yarp::test::PointD& y);
  virtual bool read(yarp::os::ConnectionReader& connection) override;
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif
