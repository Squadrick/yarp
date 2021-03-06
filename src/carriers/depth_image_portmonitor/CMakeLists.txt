# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(depthimage TYPE DepthImageConverter
                               INCLUDE DepthImage.h
                               CATEGORY portmonitor
                               DEPENDS "ENABLE_yarpcar_portmonitor")

if(NOT SKIP_depthimage)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(yarp_pm_depthimage
                  DepthImage.cpp
                  DepthImage.h)
  target_link_libraries(yarp_pm_depthimage PRIVATE YARP::YARP_OS
                                                   YARP::YARP_sig)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_OS
                                                      YARP_sig)

  yarp_install(TARGETS yarp_pm_depthimage
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES depthimage.ini
               COMPONENT ${YARP_PLUGIN_MASTER}
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_pm_depthimage PROPERTY FOLDER "Plugins/Port Monitor")
endif()
