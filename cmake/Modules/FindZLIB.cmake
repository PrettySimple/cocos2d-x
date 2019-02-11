#.rst:
# FindZLIB
# --------
#
# Find ZLIB
#
# Find the native ZLIB includes and library This module defines
#
# ::
#
#   ZLIB_INCLUDE_DIRS, where to find zlib.h, etc.
#   ZLIB_LIBRARIES, the libraries needed to use ZLIB.
#   ZLIB_FOUND, If false, do not try to use ZLIB.
#
# also defined, but not for general use are
#
# ::
#
#   ZLIB_LIBRARY, where to find the ZLIB library.

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

if(NOT EMSCRIPTEN)
	find_path(ZLIB_INCLUDE_DIR zlib.h)

	set(ZLIB_NAMES ${ZLIB_NAMES} z)
	find_library(ZLIB_LIBRARY NAMES ${ZLIB_NAMES})

	# handle the QUIETLY and REQUIRED arguments and set ZLIB_FOUND to TRUE if
	# all listed variables are TRUE
	include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZLIB DEFAULT_MSG ZLIB_LIBRARY ZLIB_INCLUDE_DIR)

	if(ZLIB_FOUND)
	  set(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR})
	  set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
	endif()
endif()

mark_as_advanced(ZLIB_LIBRARY ZLIB_INCLUDE_DIRS)
