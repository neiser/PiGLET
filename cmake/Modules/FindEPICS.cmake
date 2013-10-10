# - Find EPICS instalation
#
# Variables defined by this module:
#
#   EPICS_FOUND               System has ROOT, this means the root-config 
#                            executable was found.
#
#   EPICS_INCLUDES            Same as above,
#
#   EPICS_LIBRARIES           Link to these to use the ROOT libraries, not cached
#
#   EPICS_LIBRARY_DIR         The path to where the ROOT library files are.
#

Message(STATUS "Looking for EPICS...")

# this path should contain the "base" and "extensions" directory
Set(EPICS_BASE_SEARCHPATH
  $ENV{EPICS_BASE}
  /home/epics/base
  /opt/epics/base
)

# this search path is also appended by ${EPICS_BASE}/../extensions
# if EPICS_BASE was found at all
Set(EPICS_EXTENSIONS_SEARCHPATH
  $ENV{EPICS_EXTENSIONS}
)

Set(EPICS_FOUND FALSE)
Set(EPICS_DEFINITIONS "")

find_path(EPICS_BASE NAMES EpicsHostArch 
             PATHS ${EPICS_BASE_SEARCHPATH}
             PATH_SUFFIXES startup
             NO_DEFAULT_PATH
            )
if(NOT EPICS_BASE)
  Message(STATUS "Looking for EPICS... - EPICS_BASE not found")
  Message(STATUS "Looking for EPICS... - Environment EPICS_BASE set?")
  if(EPICS_FIND_REQUIRED)    
    message(FATAL_ERROR "Stopping here.")
  endif()
  return()
endif()

# strip off subdirectory "startup"
get_filename_component(EPICS_BASE ${EPICS_BASE} PATH)

set(EPICS_HOST_ARCH_EXE "${EPICS_BASE}/startup/EpicsHostArch")

Execute_Process(COMMAND ${EPICS_HOST_ARCH_EXE} 
  OUTPUT_VARIABLE EPICS_HOST_ARCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

if(NOT EPICS_HOST_ARCH)
  message(FATAL_ERROR "Cannot determine EPICS host arch")  
endif()

message(STATUS "Looking for EPICS... - Found ${EPICS_BASE}")
message(STATUS "Looking for EPICS... - Found host arch ${EPICS_HOST_ARCH}")

# find the includes directories
find_path(EPICS_BASE_INCLUDE NAMES tsDefs.h 
  PATHS "${EPICS_BASE}/include"
  NO_DEFAULT_PATH
  )
if(NOT EPICS_BASE_INCLUDE)
  message(FATAL_ERROR "No EPICS base include found. EPICS incomplete?")
endif()

find_path(EPICS_BASE_INCLUDE_OS NAMES osdTime.h 
  PATHS "${EPICS_BASE_INCLUDE}/os/${CMAKE_SYSTEM_NAME}"
  NO_DEFAULT_PATH
  )
if(NOT EPICS_BASE_INCLUDE_OS)
  message(FATAL_ERROR "No EPICS base include (os-dependent) found. "
    "EPICS incomplete or unsupported platform?")
endif()


# find required libraries
# -lca -lCom -lezca
# this is a bit verbose (the next library needed will trigger macro creation :)
set(EPICS_LIBRARY_DIR ${EPICS_BASE}/lib/${EPICS_HOST_ARCH}
  ${EPICS_EXTENSIONS}/lib/${EPICS_HOST_ARCH})

find_library(EPICS_LIB_CA NAMES ca
  PATHS ${EPICS_LIBRARY_DIR}
  NO_DEFAULT_PATH)  
if(NOT EPICS_LIB_CA)
  message(FATAL_ERROR "EPICS libca not found. "
    "EPICS not correctly installed?")
else()
  list(APPEND EPICS_LIBRARIES ${EPICS_LIB_CA})
endif()

find_library(EPICS_LIB_COM NAMES Com
  PATHS ${EPICS_LIBRARY_DIR}
  NO_DEFAULT_PATH)  
if(NOT EPICS_LIB_COM)
  message(FATAL_ERROR "EPICS libCom not found. "
    "EPICS not correctly installed?")
else()
  list(APPEND EPICS_LIBRARIES ${EPICS_LIB_COM})
endif()



set(EPICS_INCLUDES ${EPICS_BASE_INCLUDE} ${EPICS_BASE_INCLUDE_OS})
set(EPICS_FOUND TRUE)
