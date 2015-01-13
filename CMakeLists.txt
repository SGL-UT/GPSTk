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
option( PYTHON_USER_INSTALL "HELP: PYTHON_USER_INSTALL: SWITCH, Default = OFF, Install GPSTk python package into user home file tree instead of a system path." OFF )

#----------------------------------------
# Debug Messages
#
# The CMake command-line tool, message( STATUS ) displays 
# messages on stdout and all other message types on stderr. 
#----------------------------------------

if( DEBUG_SWITCH )
    message( STATUS "DEBUG: CMAKE_SYSTEM             = ${CMAKE_SYSTEM}" )      # e.g., Linux-3.2.0
    message( STATUS "DEBUG: CMAKE_SYSTEM_NAME        = ${CMAKE_SYSTEM_NAME}" ) # e.g., Linux
    message( STATUS "DEBUG: CMAKE_COMMAND            = ${CMAKE_COMMAND}" )     # e.g., /usr/bin/cmake
    message( STATUS "DEBUG: CMAKE_VERSION            = ${CMAKE_VERSION}" )     # e.g., 2.8.9
    message( STATUS "DEBUG: CMAKE_BUILD_TOOL         = ${CMAKE_BUILD_TOOL}" )  # e.g., /usr/bin/make
    message( STATUS "DEBUG: CMAKE_CURRENT_LIST_DIR   = ${CMAKE_CURRENT_LIST_DIR}" )    # e.g., $HOME/git/gpstk
    message( STATUS "DEBUG: CMAKE_CURRENT_LIST_FILE  = ${CMAKE_CURRENT_LIST_FILE}" )   # e.g., $HOME/git/gpstk/CMakeLists.txt
    message( STATUS "DEBUG: CMAKE_SOURCE_DIR         = ${CMAKE_SOURCE_DIR}" )          # e.g., $HOME/git/gpstk
    message( STATUS "DEBUG: CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}" )  # e.g., $HOME/git/gpstk
    message( STATUS "DEBUG: CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}" )  # e.g., $HOME/git/gpstk/build
    message( STATUS "DEBUG: PROJECT_NAME             = ${PROJECT_NAME}" )              # e.g., gpstk
    message( STATUS "DEBUG: PROJECT_BINARY_DIR       = ${PROJECT_BINARY_DIR}" )        # e.g., $HOME/git/gpstk/build
    message( STATUS "DEBUG: PROJECT_SOURCE_DIR       = ${PROJECT_SOURCE_DIR}" )        # e.g., $HOME/git/gpstk
    message( STATUS "DEBUG: CMAKE_INSTALL_DIR        = ${CMAKE_INSTALL_DIR}" )         # e.g., /usr
    message( STATUS "DEBUG: CMAKE_INSTALL_PREFIX     = ${CMAKE_INSTALL_PREFIX}" )      # e.g., $HOME/.local/gpstk
endif()

#----------------------------------------
# Set Platform-Dependent Compiler options
#----------------------------------------

if( UNIX )

    # Non-Windows platforms will use shared/dynamic libraries, not static
    set( STADYN "SHARED" )

    if( APPLE )
		# OSX Compiler Options
        set( CMAKE_SHARED_LIBRARY_SUFFIX .dylib )
        set( CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib" )
        set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -shared" )
    elseif( ${CMAKE_SYSTEM_NAME} MATCHES "SunOS" )
        if( NOT CMAKE_COMPILER_IS_GNUCC )
			# Solaris Compiler Options
            set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -mt" )
            set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -compat=5 -erroff=hidevf,wvarhidemem,badargtype2w" )
        endif()
    else()
        set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -shared" )
    endif()

elseif( WIN32 )

    # Windows platforms will use static libraries, not shared/dynamic
    set( STADYN "STATIC" )

    if( MSVC11 )
		#Compiler Options for Microsoft Visual Studio 11 (2012)
        add_definitions( /MP /D_SCL_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_WARNINGS /D_USE_MATH_DEFINES /EHsc /GR /wd"4274"
            /wd"4503" /wd"4290" /wd"4267" /wd"4250" /wd"4244" /wd"4101" /wd"4800" /wd"4068" )
    elseif( MSVC10 ) 
		#Compiler Options for Microsoft Visual Studio 10 (2010)
        include_directories( "C:/Program\ Files\ (x86)/GnuWin32/include" )
        link_directories( "C:/Program\ Files\ (x86)/GnuWin32/lib" )
        add_definitions( /MP /D_SCL_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_WARNINGS /D_USE_MATH_DEFINES /EHsc /GR /wd"4274"
            /wd"4503" /wd"4290" /wd"4267" /wd"4250" /wd"4244" /wd"4101" /wd"4800" /wd"4068" )
    endif( MSVC11 )

endif( UNIX )

#----------------------------------------
# Set Build path options
#----------------------------------------

# Use, i.e. don't skip the full RPATH for the build tree
set( CMAKE_SKIP_BUILD_RPATH FALSE )

# When building, don't use the install RPATH
# (but later on when installing)
set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )

set( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}:$ORIGIN/../lib" )

# Add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

# The RPATH to be used when installing, but only if it's not a system directory
list( FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}:$ORIGIN/../lib" isSystemDir )
if( "${isSystemDir}" STREQUAL "-1" )
   set( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}:$ORIGIN/../lib" )
endif( "${isSystemDir}" STREQUAL "-1" )

#----------------------------------------
# Debug Verbose: print all cmake variables
#----------------------------------------

if( DEBUG_VERBOSE )
	get_cmake_property( _variableNames VARIABLES )
    foreach( _variableName ${_variableNames} )
        message( STATUS "---- DEBUG VERBOSE: ${_variableName} = ${${_variableName}}" )
	endforeach()
endif()

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

if (TEST_SWITCH)
  enable_testing()
  set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules )
  find_package( CppUnit )
  if (CPPUNIT_FOUND)
    include_directories( ${CPPUNIT_INCLUDE_DIRS} )
    link_directories( ${CPPUNIT_LIBRARIES} )
    target_link_libraries(gpstk cppunit)
  else ()
    message(STATUS "CppUnit not installed.  Please install and try again." )
  endif ()
endif ()

#----------------------------------------
# Add sub-directories
#----------------------------------------

add_subdirectory( core )

if( BUILD_EXT )
   add_subdirectory( ext )
endif()
	
add_subdirectory( swig )
add_subdirectory( examples )

#----------------------------------------
# Experimental: build debs, tgz packages
# Usage: "$ make package"
#----------------------------------------
	
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "GPSTk libraries and applications for GNSS processing.") 
set( CPACK_PACKAGE_VENDOR "ARL:UT SGL" )
set( CPACK_PACKAGE_CONTACT "Bryan Parsons" )
set( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt" )
set( CPACK_PACKAGE_VERSION_MAJOR "2" )
set( CPACK_PACKAGE_VERSION_MINOR "3" )
set( CPACK_PACKAGE_VERSION_PATCH "1" )
set( CPACK_INCLUDE_TOPLEVEL_DIRECTORY "OFF" )

set( CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.13)" )
set( CPACK_DEBIAN_SECTION "stable" )
set( CPACK_DEBIAN_PACKAGE_SECTION "science" )

set( CPACK_GENERATOR "DEB;TGZ" )
include( CPack )

#------------------------------------------------------------
# The End
#------------------------------------------------------------
