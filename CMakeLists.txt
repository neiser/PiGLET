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
  include_directories(/usr/local/include
    /opt/vc/include
    /opt/vc/include/interface/vcos/pthreads
    /opt/vc/include/interface/vmcs_host/linux)
  link_directories(/opt/vc/lib)
  set(MYLIBS EGL GLESv2 bcm_host m)
else()
  
endif()

aux_source_directory(${CMAKE_SOURCE_DIR} SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_library(${PROJECT_NAME} ${MYLIBS})
