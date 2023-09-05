# Locate the OPENGL library
#
# This module defines the following variables:
#
# OPENGL_LIBRARY the name of the library;
# OPENGL_INCLUDE_DIR where to find glfw include files.
# OPENGL_FOUND true if both the OPENGL_LIBRARY and OPENGL_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you can define a
# variable called OPENGL_ROOT which points to the root of the glfw library
# installation.
#
# default search dirs
# 
# Cmake file from: https://github.com/daw42/glslcookbook

set( _OPENGL_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/includes"
)
set( _OPENGL_LIB_SEARCH_DIRS
"/usr/lib"
"/usr/local/lib"
"${CMAKE_SOURCE_DIR}/lib"
)

# Check environment for root search directory
set( _OPENGL_ENV_ROOT $ENV{OPENGL_ROOT} )
if( NOT OPENGL_ROOT AND _OPENGL_ENV_ROOT )
	set(OPENGL_ROOT ${_OPENGL_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( OPENGL_ROOT )
	list( INSERT _OPENGL_HEADER_SEARCH_DIRS 0 "${OPENGL_ROOT}/include" )
	list( INSERT _OPENGL_LIB_SEARCH_DIRS 0 "${OPENGL_ROOT}/lib" )
endif()

# Search for the header
FIND_PATH(OPENGL_INCLUDE_DIR "GL/gl.h"
PATHS ${_OPENGL_HEADER_SEARCH_DIRS} NO_DEFAULT_PATH)

# Search for the library
FIND_LIBRARY(OPENGL_LIBRARY NAMES GL
PATHS ${_OPENGL_LIB_SEARCH_DIRS} NO_DEFAULT_PATH)
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OPENGL DEFAULT_MSG
OPENGL_LIBRARY OPENGL_INCLUDE_DIR)
