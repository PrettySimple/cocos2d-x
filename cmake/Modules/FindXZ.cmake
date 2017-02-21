#.rst:
# FindXZ
# --------
#
# Find XZ
#
# Find the native XZ includes and library This module defines
#
# ::
#
#   XZLIB_INCLUDE_DIRS, where to find lzma.h, etc.
#   XZLIB_LIBRARIES, the libraries needed to use XZ.
#   XZLIB_FOUND, If false, do not try to use XZ.
#
# also defined, but not for general use are
#
# ::
#
#   XZLIB_LIBRARY, where to find the XZ library.

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

find_path(XZ_INCLUDE_DIR lzma.h)

set(XZLIB_NAMES ${XZLIB_NAMES} lzma)
find_library(XZLIB_LIBRARY NAMES ${XZLIB_NAMES})

# handle the QUIETLY and REQUIRED arguments and set ZLIB_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(XZ DEFAULT_MSG XZLIB_LIBRARY XZLIB_INCLUDE_DIR)

if(XZ_FOUND)
  set(XZLIB_INCLUDE_DIRS ${XZLIB_INCLUDE_DIR})
  set(XZLIB_LIBRARIES ${XZLIB_LIBRARY})
endif()

mark_as_advanced(XZLIB_LIBRARY XZLIB_INCLUDE_DIRS)
