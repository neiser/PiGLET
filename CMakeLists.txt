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
# and also sets ARCH_LIBS including appropiate
# OpenGL (ES) support
# and ARCH_SRC_LIST 
include(cmake/DetectArch.cmake)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-Wall")
else()
  message(FATAL_ERROR "This project can only be built with GNU g++!")
endif()


# we need EPICS (the FindEPICS will fail)
find_package(EPICS REQUIRED)
include_directories(${EPICS_INCLUDES})
link_directories(${EPICS_LIBRARY_DIR})

# then all other needed libraries
find_library(M_LIB m REQUIRED)
find_library(RT_LIB rt REQUIRED)

find_package(Threads REQUIRED)

find_package(ImageMagick COMPONENTS MagickWand REQUIRED)
add_definitions(-DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=8)
include_directories(${ImageMagick_INCLUDE_DIRS})

# PulseAudio is a bit more complicated,
# since we want to compile the soundfiles
# into the binary. We use the helper bin2c for that.
find_package(PulseAudio REQUIRED)
ADD_EXECUTABLE(bin2c sound/bin2c.c)
file(GLOB WAV_LIST RELATIVE ${CMAKE_SOURCE_DIR} sound/*.wav)
set(SOUND_OUT_DIR ${CMAKE_BINARY_DIR}/sound)
file(MAKE_DIRECTORY ${SOUND_OUT_DIR})
include_directories(${SOUND_OUT_DIR})

set(f_src "${SOUND_OUT_DIR}/wavfiles.c")
set(f_header "${SOUND_OUT_DIR}/wavfiles.h")
list(APPEND WAV_SRC_LIST ${f_header})
list(APPEND WAV_SRC_LIST ${f_src}) 
ADD_CUSTOM_COMMAND(
  OUTPUT ${f_src} ${f_header}
  COMMAND bin2c -m -o ${f_src} -d ${f_header} ${WAV_LIST}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  DEPENDS bin2c ${WAV_LIST}
  )

# the sources and includes itself
aux_source_directory(src SRC_LIST)
file(GLOB HEADER_LIST include/*.h)
include_directories(include)

# don't forget a handy config.h file
configure_file(cmake/config.h.in ${CMAKE_BINARY_DIR}/include/config.h @ONLY)
include_directories(${CMAKE_BINARY_DIR}/include)

# build the exe
add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADER_LIST} ${ARCH_SRC_LIST} ${WAV_SRC_LIST})
target_link_libraries(${PROJECT_NAME}
  ${ARCH_LIBS}
  ${M_LIB} ${RT_LIB}
  ${ImageMagick_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${EPICS_LIBRARIES}
  ${PULSEAUDIO_LIBRARY}
)
