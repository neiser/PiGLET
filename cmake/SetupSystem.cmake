# by default, we build for "normal" environments
if(NOT DEFINED BUILD_FOR_PI)
  message(STATUS "Auto-detecting system...")
  find_file(BCM_HOST_H bcm_host.h HINTS /opt/vc/include)
  if(BCM_HOST_H)
    set(BUILD_FOR_PI "ON" CACHE INTERNAL "")
    message(STATUS "Found bcm_host.h! It's a Raspberry Pi! We use GLES!")
  else()
    set(BUILD_FOR_PI "OFF" CACHE INTERNAL "")
    message(STATUS "It's a standard system. We use GLUT!")
  endif()
endif()

if(BUILD_FOR_PI)
  set(SYSTEM_DIR ${CMAKE_SOURCE_DIR}/SystemGLES)
  
  include_directories(/usr/local/include
    /opt/vc/include
    /opt/vc/include/interface/vcos/pthreads
    /opt/vc/include/interface/vmcs_host/linux)
  link_directories(/opt/vc/lib)
  set(SYSTEM_LIBS EGL GLESv2 bcm_host)
else()
  set(SYSTEM_DIR ${CMAKE_SOURCE_DIR}/SystemGLUT)

  find_package(OpenGL REQUIRED)
  find_package(GLUT REQUIRED)
  include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
  set(SYSTEM_LIBS ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
endif()

# for both systems, we add the sources and set the include dir
include_directories(${SYSTEM_DIR})
aux_source_directory(${SYSTEM_DIR} SYSTEM_SRC_LIST)