project(PiGlPlot)
cmake_minimum_required(VERSION 2.8)

# check for in-source build, forbid it!
if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR AND NOT MSVC_IDE)
  message(FATAL_ERROR "\nIn-source build attempt detected!\n"
    "Please create a new directory (e.g. build) and run `cmake ..`.\n"
    "Also don't forget to delete the created CMakeCache.txt and CMakeFiles dir"
    )
endif()

# by default, we build for "normal" environments
if(NOT DEFINED BUILD_FOR_PI)
	set(BUILD_FOR_PI 0)
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

# then all other common sources
aux_source_directory(${CMAKE_SOURCE_DIR} SRC_LIST)
include_directories(${CMAKE_SOURCE_DIR})

# build the exe
add_executable(${PROJECT_NAME} ${SRC_LIST} ${SYSTEM_SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${SYSTEM_LIBS} m)
