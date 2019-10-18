#.rst:
# FindBZ2
# --------
#
# Find BZ2
#
# Find the native BZ2 includes and library This module defines
#
# ::
#
#   BZ2LIB_INCLUDE_DIRS, where to find bzlib.h, etc.
#   BZ2LIB_LIBRARIES, the libraries needed to use BZ2.
#   BZ2LIB_FOUND, If false, do not try to use BZ2.
#
# also defined, but not for general use are
#
# ::
#
#   BZ2LIB_LIBRARY, where to find the BZ2 library.

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

find_path(BZ2LIB_INCLUDE_DIR bzlib.h
  HINTS ENV WEBP_DIR
  PATH_SUFFIXES include/webp include
)

message("${BZ2LIB_INCLUDE_DIR}")

set(BZ2LIB_NAMES ${BZIP2LIB_NAMES} bz2)
find_library(BZ2LIB_LIBRARY NAMES ${BZ2LIB_NAMES})

# handle the QUIETLY and REQUIRED arguments and set ZLIB_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BZ2LIB DEFAULT_MSG BZ2LIB_LIBRARY BZ2LIB_INCLUDE_DIR)

if(BZ2LIB_FOUND)
  set(BZ2LIB_INCLUDE_DIRS ${BZ2LIB_INCLUDE_DIR})
  set(BZ2LIB_LIBRARIES ${BZ2LIB_LIBRARY})
endif()

mark_as_advanced(BZ2LIB_LIBRARY BZ2LIB_INCLUDE_DIRS)
