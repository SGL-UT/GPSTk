


#----------------------------------------
# Debug Messages
#
# The CMake command-line tool, message( STATUS ) displays 
# messages on stdout and all other message types on stderr. 
#----------------------------------------

if( DEBUG_SWITCH )
    message( STATUS "DEBUG: Included CMake file BuildSetup.cmake" )
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
    if( BUILD_PYTHON )
        message( STATUS "DEBUG: PYTHON_INSTALL_PREFIX    = ${PYTHON_INSTALL_PREFIX}" )     # e.g., $HOME/.local
    endif()
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
# Get CMake vars into C++
#----------------------------------------

configure_file( "${PROJECT_SOURCE_DIR}/build_config.h.in" "${PROJECT_BINARY_DIR}/generated/build_config.h" )
include_directories( "${PROJECT_BINARY_DIR}/generated/" ) 
install( FILES "${PROJECT_BINARY_DIR}/generated/build_config.h" DESTINATION include )


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

#----------------------------------------
# The End
#----------------------------------------
