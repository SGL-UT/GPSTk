#----------------------------------------
# GPSTk: CMake Input: $GPSTK/CMakeLists.txt
#----------------------------------------

cmake_minimum_required( VERSION 2.8.4 )

project( gpstk )

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
# Build Config
#----------------------------------------

include( BuildSetup.cmake )

#----------------------------------------
# Define core library source files
#----------------------------------------

file( GLOB_RECURSE CORE_SRC "core/lib/*.cpp" )
file( GLOB_RECURSE CORE_SRC2 "core/lib/*.c" )
file( GLOB_RECURSE CORE_HDRS "core/lib/*.h" )
file( GLOB_RECURSE CORE_HDRS2 "core/lib/*.hpp" )

#----------------------------------------
# Define ext library source files
#----------------------------------------

if( BUILD_EXT )
  file( GLOB_RECURSE EXT_SRC "ext/lib/*.cpp" )
  file( GLOB_RECURSE EXT_SRC2 "ext/lib/*.c" )
  file( GLOB_RECURSE EXT_HDRS "ext/lib/*.h" )
  file( GLOB_RECURSE EXT_HDRS2 "ext/lib/*.hpp" )
endif()

#----------------------------------------
# Remove getopt.h from non-Windows installs of /ext library
#----------------------------------------

if( NOT WIN32 )
  foreach( _headerFile ${EXT_HDRS} )
    get_filename_component( _name ${_headerFile} NAME )
    if( ${_name} MATCHES "getopt.h" )
      list( REMOVE_ITEM EXT_HDRS ${_headerFile} )
    endif()
  endforeach()
endif()

#----------------------------------------
# Define core library include directories
#----------------------------------------

# initialize list of include directories
set( CORE_INCLUDE_DIRS "" )

foreach( _headerFile ${CORE_HDRS} )
    get_filename_component( _dir ${_headerFile} PATH )
    list( APPEND CORE_INCLUDE_DIRS ${_dir} )
endforeach()

foreach( _headerFile ${CORE_HDRS2} )
    get_filename_component( _dir ${_headerFile} PATH )
    list( APPEND CORE_INCLUDE_DIRS ${_dir} )
endforeach()

list( REMOVE_DUPLICATES CORE_INCLUDE_DIRS )

# Add every directory containing a header file
# to the project(gpstk) include_directories
include_directories( ${CORE_INCLUDE_DIRS} )

#----------------------------------------
# Define ext library include directories
#----------------------------------------

if( BUILD_EXT )
    # initialize list of include directories
    set( EXT_INCLUDE_DIRS "" )

    foreach( _headerFile ${EXT_HDRS} )
        get_filename_component( _dir ${_headerFile} PATH )
        get_filename_component( _name ${_headerFile} NAME )
        list( APPEND EXT_INCLUDE_DIRS ${_dir} )
    endforeach()

    foreach( _headerFile ${EXT_HDRS2} )
        get_filename_component( _dir ${_headerFile} PATH )
        list( APPEND EXT_INCLUDE_DIRS ${_dir} )
    endforeach()

    list( REMOVE_DUPLICATES EXT_INCLUDE_DIRS )

    # Add every directory containing a header file
	# to the project(gpstk) include_directories
    include_directories( ${EXT_INCLUDE_DIRS} )
endif()


#----------------------------------------
# Define core and ext library build targets
#----------------------------------------
	
if( NOT BUILD_EXT )
    add_library( gpstk ${STADYN} ${CORE_SRC} ${CORE_SRC2} )
else()
    add_library( gpstk ${STADYN} ${CORE_SRC} ${CORE_SRC2} ${EXT_SRC} ${EXT_SRC2} )
endif()

#----------------------------------------
# Define core and ext library install targets
#----------------------------------------
#
# Install target destination paths assume CMAKE_INSTALL_PREFIX is prepended.
# Example: install( FILES ${CORE_HDRS} DESTINATION include ) copies files ${CORE_HDRS}
#          into the path ${CMAKE_INSTALL_PREFIX}/include/${CORE_HDRS}

# Library install target
install( TARGETS gpstk DESTINATION lib )

# Header file install target
if( NOT BUILD_EXT )
    install( FILES ${CORE_HDRS} ${CORE_HDRS2} DESTINATION include )
else()
    install( FILES ${CORE_HDRS} ${CORE_HDRS2} ${EXT_HDRS} ${EXT_HDRS2} DESTINATION include )
endif()

#----------------------------------------
# Test target
#----------------------------------------

if( TEST_SWITCH )
	# CMake command enable_testing() enables capability 
	# to use CMake add_test() command for this directory and below. 
	# This adds another build target, which is "test" for 
	# Makefile generators, or "RUN_TESTS" for IDEs (e.g. Visual Studio).
	# From that point on, you can use the ADD_TEST() command 
	# to add tests cases to the project build target test:
	#   add_test( testname Exename arg1 arg2 ... )
	# Must call enable_test() prior to any add_subdirectory() 
	# that is intended to contain test cases.
	# Currently, there are tests under /core and /tests subdirectories.
    enable_testing()
endif()

#----------------------------------------
# Test Switch
#----------------------------------------

if( TEST_SWITCH )
  enable_testing()
  set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules )
  find_package( CppUnit )
  if (CPPUNIT_FOUND)
    include_directories( ${CPPUNIT_INCLUDE_DIRS} )
    link_directories( ${CPPUNIT_LIBRARIES} )
    target_link_libraries(gpstk cppunit)
  else( )
    message( STATUS "CppUnit not installed.  Please install and try again." )
  endif( )
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



#------------------------------------------------------------
# The End
#------------------------------------------------------------
