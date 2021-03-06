# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(h264
                    CATEGORY carrier
                    TYPE yarp::os::H264Carrier
                    INCLUDE H264Carrier.h
                    EXTRA_CONFIG CODE=""
                    DEPENDS "GLIB2_FOUND;GSTREAMER_FOUND;GSTREAMER_APP_LIBRARY_FOUND")

if(NOT SKIP_h264)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(yarp_h264
                  H264Carrier.h
                  H264Carrier.cpp
                  H264Stream.h
                  H264Stream.cpp
                  H264Decoder.cpp
                  H264Decoder.h)

  target_link_libraries(yarp_h264 PRIVATE YARP::YARP_OS
                                          YARP::YARP_sig
                                          YARP::YARP_wire_rep_utils)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_OS
                                                      YARP_sig
                                                      YARP_wire_rep_utils)

  # GLIB2 is required by GStreamer
  target_link_libraries(yarp_h264 PRIVATE ${GLIB2_LIBRARIES})
  target_include_directories(yarp_h264 SYSTEM PRIVATE ${GLIB2_INCLUDE_DIR})

  target_include_directories(yarp_h264 SYSTEM PRIVATE ${GSTREAMER_INCLUDE_DIRS})
  target_link_libraries(yarp_h264 PRIVATE ${GSTREAMER_LIBRARY})

  target_include_directories(yarp_h264 SYSTEM PRIVATE ${GSTREAMER_app_INCLUDE_DIR})
  target_link_libraries(yarp_h264 PRIVATE ${GSTREAMER_APP_LIBRARY})

  yarp_install(TARGETS yarp_h264
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES h264.ini
               COMPONENT ${YARP_PLUGIN_MASTER}
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_h264 PROPERTY FOLDER "Plugins/Carrier")
endif()
