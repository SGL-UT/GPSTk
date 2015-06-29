#============================================================
# Name    = $GPSTK/CMakeLists.txt
# Purpose = Generator of build framework (e.g. Makefiles) for GPSTk
# Notes   = This is the top-level CMake input file
#           Depends on $GPSTK/BuildSetup.cmake
#           Is dependend on by $GPSTK/build.sh
#============================================================

cmake_minimum_required( VERSION 2.8.4 )

project( GPSTK )
set( GPSTK_VERSION_MAJOR "2" )
set( GPSTK_VERSION_MINOR "5" )
set( GPSTK_VERSION_PATCH "0" )
set( GPSTK_VERSION "${GPSTK_VERSION_MAJOR}.${GPSTK_VERSION_MINOR}.${GPSTK_VERSION_PATCH}" )

#============================================================
# Set up
#============================================================

#----------------------------------------
# Determine Installation Path Prefix
#     Path to which we will install the
#     gpstk /lib, /bin, and /include dirs.
#----------------------------------------

if( DEFINED CMAKE_INSTALL_PREFIX )
    message( STATUS "CMAKE_INSTALL_PREFIX was defined by user with the cmake command line. CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
elseif( DEFINED ENV{gpstk} )
    set( CMAKE_INSTALL_PREFIX $ENV{gpstk} )
    message( STATUS "CMAKE_INSTALL_PREFIX was defined the environment variable ENV{gpstk}. CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX} " )
else()
    message( STATUS "CMAKE_INSTALL_PREFIX was not defined by any supported method." )
	message( STATUS "Either set an environment variable ENV{gpstk} or pass in a cmake variable as -DCMAKE_INSTALL_PREFIX")
    message( FATAL_ERROR "CMAKE_INSTALL_PREFIX must be set by the user. Exitting now!" )
    return()
endif()

# This sets up veriables contining GNU standard installation locations.
include( GNUInstallDirs )
# For now, override to use 'lib' for the library install dir.
set( CMAKE_INSTALL_LIBDIR lib )

# Set a filename for collecting exported targets.
set( EXPORT_TARGETS_FILENAME "GPSTKTargets" )

#----------------------------------------
# Option Switches
#
# Usage: option( <option_variable> "help string" [initial value] )
#----------------------------------------

option( DEBUG_SWITCH "HELP: DEBUG_SWITCH: Default = OFF, print some CMake variable values to stdout." OFF )
option( DEBUG_VERBOSE "HELP: DEBUG_VERBOSE: Default = OFF, print all CMake variable values." OFF )
option( BUILD_EXT "HELP: BUILD_EXT: SWITCH, Default = OFF, Build the ext library, in addition to the core library." OFF )
option( TEST_SWITCH "HELP: TEST_SWITCH: SWITCH, Default = OFF, Turn on test mode." OFF )
option( BUILD_PYTHON "HELP: BUILD_PYTHON: SWITCH, Default = OFF, Turn on processing of python extension package." OFF )

#----------------------------------------
# Pull in Build Config
#----------------------------------------

include( BuildSetup.cmake )

#============================================================
# Core Library Target Files
#============================================================

#----------------------------------------
# Define $GPSTK/core/ library source files
#----------------------------------------

file( GLOB_RECURSE CORE_SRC_FILES "core/lib/*.cpp" "core/lib/*.c" )
file( GLOB_RECURSE CORE_INC_FILES "core/lib/*.h" "core/lib/*.hpp" )

#----------------------------------------
# Define /core library include directories
#----------------------------------------

# initialize list of include directories
set( CORE_INC_DIRS "" )

foreach( _headerFile ${CORE_INC_FILES} )
    get_filename_component( _dir ${_headerFile} PATH )
    list( APPEND CORE_INC_DIRS ${_dir} )
endforeach()

list( REMOVE_DUPLICATES CORE_INC_DIRS )

# Add every directory containing a header file
# to the project(gpstk) include_directories
include_directories( ${CORE_INC_DIRS} )

# define src and include files needed to build library target
set( GPSTK_SRC_FILES "" )
set( GPSTK_INC_FILES "" )
list( APPEND GPSTK_SRC_FILES ${CORE_SRC_FILES} )
list( APPEND GPSTK_INC_FILES ${CORE_INC_FILES} )

#============================================================
# Define $GPSTK/ext/ additions to Library Target Files
#============================================================

if( BUILD_EXT )
  file( GLOB_RECURSE EXT_SRC_FILES "ext/lib/*.cpp" "ext/lib/*.c" )
  file( GLOB_RECURSE EXT_INC_FILES "ext/lib/*.h" "ext/lib/*.hpp" )

  # Remove getopt.h from non-Windows installs of /ext library
  if( NOT WIN32 )
    foreach( _headerFile ${EXT_INC_FILES} )
      get_filename_component( _name ${_headerFile} NAME )
      if( ${_name} MATCHES "getopt.h" )
        list( REMOVE_ITEM EXT_INC_FILES ${_headerFile} )
      endif()
    endforeach()
  endif()

  # Define ext library include directories
  set( EXT_INC_DIRS "" )

  foreach( _headerFile ${EXT_INC_FILES} )
    get_filename_component( _dir ${_headerFile} PATH )
    get_filename_component( _name ${_headerFile} NAME )
    list( APPEND EXT_INC_DIRS ${_dir} )
  endforeach()

  list( REMOVE_DUPLICATES EXT_INC_DIRS )

  # Add every directory containing a header file
  # to the project(gpstk) include_directories
  include_directories( ${EXT_INC_DIRS} )

  # append ext include files needed to build library target
  list( APPEND GPSTK_SRC_FILES ${EXT_SRC_FILES} )
  list( APPEND GPSTK_INC_FILES ${EXT_INC_FILES} )

endif()

#============================================================
# GPSTK Library, Build and Install Targets
#============================================================

# GPSTk shared-object library (e.g. libgpstk.so) build target
add_library( gpstk ${STADYN} ${GPSTK_SRC_FILES} )

# GPSTk library install target
install( TARGETS gpstk DESTINATION "${CMAKE_INSTALL_LIBDIR}" EXPORT "${EXPORT_TARGETS_FILENAME}" )

# GPSTk header file install target
install( FILES ${GPSTK_INC_FILES} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" )

#============================================================
# Testing
#============================================================

# CMake command enable_testing() enables capability 
# to use CMake add_test() command for this directory and below. 
# This adds another build target, which is "test" for 
# Makefile generators, or "RUN_TESTS" for IDEs (e.g. Visual Studio).
# From that point on, you can use the ADD_TEST() command 
# to add tests cases to the project build target test:
#
#     add_test( testname Exename arg1 arg2 ... )
#
# Must call enable_test() prior to any add_subdirectory() 
# that is intended to contain test cases.

if( TEST_SWITCH )
  enable_testing()
  list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules )
endif( )

#----------------------------------------
# Add sub-directories
#----------------------------------------

add_subdirectory( core )

if( BUILD_EXT )
   add_subdirectory( ext )
   add_subdirectory( examples )
endif()
	
if( BUILD_PYTHON )
   add_subdirectory( swig )
endif()

#----------------------------------------
# Export the project import cmake files.
#----------------------------------------

include( CMakePackageConfigHelpers )
set( INSTALL_CONFIG_DIR ${CMAKE_INSTALL_DATADIR}/cmake/GPSTK)
configure_package_config_file( ${CMAKE_CURRENT_SOURCE_DIR}/projectConfig.cmake.in 
                               ${CMAKE_CURRENT_BINARY_DIR}/GPSTKConfig.cmake
                               INSTALL_DESTINATION ${INSTALL_CONFIG_DIR}
                               PATH_VARS CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_BINDIR CMAKE_INSTALL_PREFIX INSTALL_CONFIG_DIR )
write_basic_package_version_file( ${CMAKE_CURRENT_BINARY_DIR}/GPSTKConfigVersion.cmake
                                  VERSION ${GPSTK_VERSION}
                                  COMPATIBILITY ExactVersion )
install( FILES 
           ${CMAKE_CURRENT_BINARY_DIR}/GPSTKConfig.cmake 
           ${CMAKE_CURRENT_BINARY_DIR}/GPSTKConfigVersion.cmake
         DESTINATION 
           ${INSTALL_CONFIG_DIR} )

install( EXPORT ${EXPORT_TARGETS_FILENAME} DESTINATION ${INSTALL_CONFIG_DIR} )

#============================================================
# The End
#============================================================

