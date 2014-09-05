# Find GPSTK
#
# Find the native GPSTK includes and library.
# Currently just hard-coded paths
#
#  GPSTK_INCLUDE_DIRS   - where to find GPSTK files.hpp, etc.
#  GPSTK_LIBRARIES      - List of libraries when using GPSTK.
#  GPSTK_FOUND          - True if GPSTK was "found". So, always true.
#
#=============================================================================

if( DEFINED ENV{gpstk} )
  message(STATUS "Using GPSTK defined by environment variable $ENV{gpstk}..." )
  set( GPSTK_FOUND TRUE )
  set( GPSTK_INSTALL $ENV{gpstk} )
  set( GPSTK_INCLUDE_DIRS ${GPSTK_INSTALL}/include )
  set( GPSTK_LIBRARIES ${GPSTK_INSTALL}/lib/libgpstk.so )
  set( GPSTK_LIBRARY_DIRS ${GPSTK_INSTALL}/lib )
  set( GPSTK_SO_VERSION 23 )
else( DEFINED GPSTK_INSTALL )
  message(STATUS "Using GPSTK defined by CMake variable ${GPSTK_INSTALL}..." )
  set( GPSTK_FOUND TRUE )
  set( GPSTK_INCLUDE_DIRS ${GPSTK_INSTALL}/include )
  set( GPSTK_LIBRARIES ${GPSTK_INSTALL}/lib/libgpstk.so )
  set( GPSTK_LIBRARY_DIRS ${GPSTK_INSTALL}/lib )
  set( GPSTK_SO_VERSION 23 )
elseif( )
  set( GPSTK_FOUND FALSE )
  message(WARNING "The GPSTK install path is not defined.")
  message(WARNING "You must tell CMake the full path to your GPSTk install." )
  message(WARNING "    Option 1: Define an environment variable gpstk." )
  message(WARNING "              Example: $ export gpstk=~/git/gpstk/dev/install " )
  message(WARNING "    Option 2: Define a CMake variable GPSTK_INSTALL." )
  message(WARNING "              Example: $ cmake -DGPSTK_INSTALL=~/git/gpstk/dev/install " )
  return( )
endif( )
return( )

