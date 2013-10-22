# Locate sndfile
# This module defines
# SNDFILE_LIBRARY
# SNDFILE_FOUND, if false, do not try to link to sndfile
# SNDFILE_INCLUDE_DIR, where to find the headers
#
# $SNDFILE_DIR is an environment variable that would
# correspond to the ./configure --prefix=$SNDFILE_DIR

FIND_PATH(SNDFILE_INCLUDE_DIR sndfile.h
  PATH_SUFFIXES sndfile)

find_library(SNDFILE_LIBRARY NAMES "sndfile")

SET(SNDFILE_FOUND "NO")
IF(SNDFILE_LIBRARY AND SNDFILE_INCLUDE_DIR)
  SET(SNDFILE_FOUND "YES")
ENDIF(SNDFILE_LIBRARY AND SNDFILE_INCLUDE_DIR)


