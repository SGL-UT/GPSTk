
if( DEBUG_SWITCH )
    message( STATUS "DEBUG: Included CMake file BuildSetup.cmake" )
endif()

#========================================
# Set Platform-Dependent Options
#========================================

#----------------------------------------
# Shared(Dynamic) vs. Static libraries
#----------------------------------------
if( UNIX )
    set( STADYN "SHARED" )
elseif( WIN32 )
    set( STADYN "STATIC" )
endif()

#----------------------------------------
# Platform-dependent Compiler flags
#----------------------------------------
if( ${CMAKE_SYSTEM_NAME} MATCHES "SunOS" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -compat=5 -erroff=hidevf,wvarhidemem,badargtype2w" )
    set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -mt -shared" )
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    set( CMAKE_SHARED_LIBRARY_SUFFIX .dylib )
    set( CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib" )
    set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -shared" )
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2" )
    set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -shared" )
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )

else()
    message( ERROR "ERROR: CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}, not supported. Currently supported: Linux, Darwin, SunOS, " )
endif()

#----------------------------------------
# Windows Visual Studio flags
#----------------------------------------
if( WIN32 )
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
endif()

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
# Debug Messages
#
# The CMake command-line tool, message( STATUS ) displays 
# messages on stdout and all other message types on stderr. 
#----------------------------------------

if( DEBUG_SWITCH )
    message( STATUS "DEBUG: CMAKE_SYSTEM              = ${CMAKE_SYSTEM}" )      # e.g., Linux-3.2.0
    message( STATUS "DEBUG: CMAKE_SYSTEM_NAME         = ${CMAKE_SYSTEM_NAME}" ) # e.g., Linux
    message( STATUS "DEBUG: UNIX                      = ${UNIX}" ) # e.g., is the platform UNIX ?
    message( STATUS "DEBUG: APPLE                     = ${APPLE}" ) # e.g., is the platform OSX?
    message( STATUS "DEBUG: WIN32                     = ${WIN32}" ) # e.g., is the platform Windows?
    message( STATUS "DEBUG: MSVC11                    = ${MSVC11}" ) # e.g., is the platform VisualStudio?
    message( STATUS "DEBUG: MSVC12                    = ${MSVC12}" ) # e.g., is the platform VisualStudio?
    message( STATUS "DEBUG: CMAKE_COMMAND             = ${CMAKE_COMMAND}" )     # e.g., /usr/bin/cmake
    message( STATUS "DEBUG: CMAKE_VERSION             = ${CMAKE_VERSION}" )     # e.g., 2.8.9
    message( STATUS "DEBUG: CMAKE_BUILD_TOOL          = ${CMAKE_BUILD_TOOL}" )  # e.g., /usr/bin/make
    message( STATUS "DEBUG: STADYN                    = ${STADYN}" )
    message( STATUS "DEBUG: CMAKE_COMPILER_IS_GNUCC   = ${CMAKE_COMPILER_IS_GNUCC}" )
    message( STATUS "DEBUG: CMAKE_CXX_FLAGS           = ${CMAKE_CXX_FLAGS}" )
    message( STATUS "DEBUG: CMAKE_SHARED_LINKER_FLAGS = ${CMAKE_SHARED_LINKER_FLAGS}" )
    message( STATUS "DEBUG: CMAKE_SKIP_BUILD_RPATH    = ${CMAKE_SKIP_BUILD_RPATH}" )
    message( STATUS "DEBUG: CMAKE_INSTALL_RPATH       = ${CMAKE_INSTALL_RPATH}" )
    message( STATUS "DEBUG: CMAKE_BUILD_WITH_INSTALL_RPATH = ${CMAKE_BUILD_WITH_INSTALL_RPATH}" )
    message( STATUS "DEBUG: CMAKE_INSTALL_RPATH_USE_LINK_PATH = ${CMAKE_INSTALL_RPATH_USE_LINK_PATH}" )
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
install( FILES "${PROJECT_BINARY_DIR}/generated/build_config.h" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" )


#-----------------------------------------
# Build debs, tgz packages for binaries 
# Usage: "$ make package" 
#-----------------------------------------
	

set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "GPSTk libraries and applications for GNSS processing.") 
set( CPACK_PACKAGE_VENDOR "ARL:UT SGL" )
set( CPACK_PACKAGE_CONTACT "Bryan Parsons" )
set( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt" )
set( CPACK_PACKAGE_VERSION_MAJOR "${gpstk_VERSION_MAJOR}" )
set( CPACK_PACKAGE_VERSION_MINOR "${gpstk_VERSION_MINOR}" )
set( CPACK_PACKAGE_VERSION_PATCH "${gpstk_VERSION_PATCH}" )
set( CPACK_INCLUDE_TOPLEVEL_DIRECTORY "OFF" )

set( CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.13)" )
set( CPACK_DEBIAN_SECTION "stable" )
set( CPACK_DEBIAN_PACKAGE_SECTION "science" )

set( CPACK_GENERATOR "DEB;TGZ" )

#-----------------------------------------
# Build tgz packages for source release
# Usage: "$ make package_source" 
#-----------------------------------------

set( CPACK_SOURCE_IGNORE_FILES "/build/")
set( CPACK_SOURCE_GENERATOR "TGZ")

include( CPack )

#----------------------------------------
# The End
#----------------------------------------
