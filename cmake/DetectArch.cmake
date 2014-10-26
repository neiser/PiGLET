# by default, we build for "normal" environments
if(NOT DEFINED BUILD_FOR_PI)
  message(STATUS "Detecting architecture...")
  find_file(BCM_HOST_H bcm_host.h HINTS /opt/vc/include)
  if(BCM_HOST_H)
    set(BUILD_FOR_PI "ON" CACHE INTERNAL "")
    message(STATUS "Found bcm_host.h! It's a Raspberry Pi. We use GLES.")
  else()
    set(BUILD_FOR_PI "OFF" CACHE INTERNAL "")
    message(STATUS "No bcm_host.h found. We use GLUT.")
  endif()
endif()

if(BUILD_FOR_PI)
  set(ARCH_DIR ${CMAKE_SOURCE_DIR}/arch/GLES)
  
  include_directories(/usr/local/include
    /opt/vc/include
    /opt/vc/include/interface/vcos/pthreads
    /opt/vc/include/interface/vmcs_host/linux)
  link_directories(/opt/vc/lib)
  set(ARCH_LIBS EGL GLESv2 bcm_host)
  add_definitions(-DBUILD_PI)
else()
  set(ARCH_DIR ${CMAKE_SOURCE_DIR}/arch/GLUT)

  find_package(OpenGL REQUIRED)
  find_package(GLUT REQUIRED)
  include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
  set(ARCH_LIBS ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
  add_definitions(-DBUILD_NONPI)
endif()

# for all archs, we add the sources 
# and set the include dir already here
include_directories(${ARCH_DIR} arch) # add arch dir for arch_common.h
aux_source_directory(${ARCH_DIR} ARCH_SRC_LIST)
