project(PiGLET)
cmake_minimum_required(VERSION 2.8)

# check for in-source build, forbid it!
if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR AND NOT MSVC_IDE)
  message(FATAL_ERROR "\nIn-source build attempt detected!\n"
    "Please create a new directory (e.g. build) and run `cmake ..`.\n"
    "Also don't forget to delete the created CMakeCache.txt and CMakeFiles dir"
    )
endif()

# include some extra Find* utilities
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# this determines if we're on an Raspberry Pi
# and also sets SYSTEM_LIBS including appropiate
# OpenGL (ES) support
# and SYSTEM_SRC_LIST 
include(cmake/SetupSystem.cmake)

# we need EPICS
find_package(EPICS)
include_directories(${EPICS_INCLUDES})
link_directories(${EPICS_LIBRARY_DIR})


# then all other needed libraries
find_library(M_LIB m REQUIRED)
find_library(RT_LIB rt REQUIRED)

find_package(Threads REQUIRED)

find_package(ImageMagick COMPONENTS MagickWand REQUIRED)
add_definitions(-DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16)
include_directories(${ImageMagick_INCLUDE_DIRS})

# the sources itself,
# we don't have an extra src directory...
aux_source_directory(. SRC_LIST)
include_directories(.)

set(CMAKE_CXX_FLAGS "-Wall")

# build the exe
add_executable(${PROJECT_NAME} ${SRC_LIST} ${SYSTEM_SRC_LIST})
target_link_libraries(${PROJECT_NAME}
  ${SYSTEM_LIBS}
  ${M_LIB} ${RT_LIB}
  ${ImageMagick_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${EPICS_LIBRARIES})
