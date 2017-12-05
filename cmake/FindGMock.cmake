##############################################################################
# @file  FindGMock.cmake
# @brief Find Google Mock package.
#
# @par Input variables:
# <table border="0">
#   <tr>
#     @tp @b GMock_DIR @endtp
#     <td>The Google Mock package files are searched under the specified
#         root directory. If they are not found there, the default search
#         paths are considered. This variable can also be set as environment
#         variable.</td>
#   </tr>
#   <tr>
#     @tp @b GMOCK_DIR @endtp
#     <td>Alternative environment variable for @p GMock_DIR.</td>
#   </tr>
#   <tr>
#     @tp @b GMock_SHARED_LIBRARIES @endtp
#     <td>Forces this module to search for shared libraries.
#         Otherwise, static libraries are preferred.</td>
#   </tr>
# </table>
#
# @par Output variables:
# <table border="0">
#   <tr>
#     @tp @b GMock_FOUND @endtp
#     <td>Whether the package was found and the following CMake variables are valid.</td>
#   </tr>
#   <tr>
#     @tp @b GMock_INCLUDE_DIR @endtp
#     <td>Package include directories.</td>
#   </tr>
#   <tr>
#     @tp @b GMock_INCLUDES @endtp
#     <td>Include directories including prerequisite libraries.</td>
#   </tr>
#   <tr>
#     @tp @b GMock_LIBRARY @endtp
#     <td>Package libraries.</td>
#   </tr>
#   <tr>
#     @tp @b GMock_LIBRARIES @endtp
#     <td>Package ibraries and prerequisite libraries.</td>
#   </tr>
# </table>
#
# @ingroup CMakeFindModules
##############################################################################

#=============================================================================
# Copyright 2011-2012 University of Pennsylvania
# Copyright 2013-2016 Andreas Schuh <andreas.schuh.84@gmail.com>
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

# ----------------------------------------------------------------------------
# initialize search
if (NOT GMock_DIR)
  if ($ENV{GMOCK_DIR})
    set (GMock_DIR "$ENV{GMOCK_DIR}" CACHE PATH "Installation prefix for Google Mock.")
  else ()
    set (GMock_DIR "$ENV{GMock_DIR}" CACHE PATH "Installation prefix for Google Mock.")
  endif ()
endif ()

set (GMock_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})

if (GMock_SHARED_LIBRARIES)
  if (WIN32)
    set (CMAKE_FIND_LIBRARY_SUFFIXES .dll)
  else ()
    set (CMAKE_FIND_LIBRARY_SUFFIXES .so)
  endif()
else ()
  if (WIN32)
    set (CMAKE_FIND_LIBRARY_SUFFIXES .lib)
  else ()
    set (CMAKE_FIND_LIBRARY_SUFFIXES .a)
  endif()
endif ()

# ----------------------------------------------------------------------------
# find paths/files
if (GMock_DIR)

  find_path (
    GMock_INCLUDE_DIR
      NAMES         gmock.h
      HINTS         "${GMock_DIR}"
      PATH_SUFFIXES "include/gmock"
      DOC           "Include directory for Google Mock."
      NO_DEFAULT_PATH
  )

  find_library (
    GMock_LIBRARY
      NAMES         gmock
      HINTS         "${GMock_DIR}"
      PATH_SUFFIXES "lib"
      DOC           "Link library for Google Mock (gmock)."
      NO_DEFAULT_PATH
  )

else ()

  find_path (
    GMock_INCLUDE_DIR
      NAMES gmock.h
      HINTS ENV C_INCLUDE_PATH ENV CXX_INCLUDE_PATH
      DOC   "Include directory for Google Mock."
  )

  find_library (
    GMock_LIBRARY
      NAMES gmock
      HINTS ENV LD_LIBRARY_PATH
      DOC   "Link library for Google Mock (gmock)."
  )

endif ()

mark_as_advanced (GMock_INCLUDE_DIR)
mark_as_advanced (GMock_LIBRARY)

# ----------------------------------------------------------------------------
# prerequisite libraries
set (GMock_INCLUDES  "${GMock_INCLUDE_DIR}")
set (GMock_LIBRARIES "${GMock_LIBRARY}")

# ----------------------------------------------------------------------------
# reset CMake variables
set (CMAKE_FIND_LIBRARY_SUFFIXES ${GTest_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})

# ----------------------------------------------------------------------------
# aliases / backwards compatibility
set (GMock_INCLUDE_DIRS "${GTest_INCLUDES}")

# ----------------------------------------------------------------------------
# handle the QUIETLY and REQUIRED arguments and set *_FOUND to TRUE
# if all listed variables are found or TRUE
include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (
  GMock
  REQUIRED_VARS
    GMock_INCLUDE_DIR
    GMock_LIBRARY
)

set (GMock_FOUND "${GMOCK_FOUND}")

# ----------------------------------------------------------------------------
# set GMock_DIR
if (NOT GMock_DIR AND GMock_FOUND)
  string (REGEX REPLACE "include(/gmock)?/?" "" GMock_PREFIX "${GMock_INCLUDE_DIR}")
  set (GMock_DIR "${GMock_PREFIX}" CACHE PATH "Installation prefix for GMock." FORCE)
  unset (GMock_PREFIX)
endif ()