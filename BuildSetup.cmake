# gpstk/BuildSetup.cmake
#========================================
# Try to keep all the platform dependent
# options in this file
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
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -erroff=hidevf,wvarhidemem,badargtype2w" )
    # add -DCMAKE_CXX_FLAGS=-std=c++03 or =-std=c++11 on the CMAKE invocation
    set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -mt -shared" )
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    if ( ${CMAKE_BUILD_TYPE} MATCHES "debug" )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address" )
    endif()
    set( CMAKE_SHARED_LIBRARY_SUFFIX .dylib )
    set( CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib" )
    set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -shared" )
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2" )
    set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -shared" )
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
else()
    message( ERROR "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}, not supported. Currently supported: Linux, Darwin, SunOS, Windows" )
endif()

#----------------------------------------
# Windows Visual Studio flags
#----------------------------------------
if( WIN32 )
    if( MSVC14 )
        #Compiler Options for Microsoft Visual Studio 14 (2015)
        add_definitions( /MP /D_SCL_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_WARNINGS /D_USE_MATH_DEFINES /EHsc /GR /wd"4274"
            /wd"4503" /wd"4290" /wd"4267" /wd"4250" /wd"4244" /wd"4101" /wd"4800" /wd"4068" )
    elseif( MSVC11 )
        #Compiler Options for Microsoft Visual Studio 11 (2012)
        add_definitions( /MP /D_SCL_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_WARNINGS /D_USE_MATH_DEFINES /EHsc /GR /wd"4274"
            /wd"4503" /wd"4290" /wd"4267" /wd"4250" /wd"4244" /wd"4101" /wd"4800" /wd"4068" )
    endif( MSVC14 )
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
if( DEBUG_SWITCH AND NOT DEBUG_VERBOSE )
    message( STATUS "CMAKE_SYSTEM              = ${CMAKE_SYSTEM}" )      # e.g., Linux-3.2.0
    message( STATUS "CMAKE_SYSTEM_NAME         = ${CMAKE_SYSTEM_NAME}" ) # e.g., Linux
    message( STATUS "UNIX                      = ${UNIX}" ) # e.g., is the platform UNIX ?
    message( STATUS "APPLE                     = ${APPLE}" ) # e.g., is the platform OSX?
    message( STATUS "WIN32                     = ${WIN32}" ) # e.g., is the platform Windows?
    message( STATUS "MSVC11                    = ${MSVC11}" ) # e.g., is the platform VisualStudio?
    message( STATUS "MSVC12                    = ${MSVC12}" ) # e.g., is the platform VisualStudio?
    message( STATUS "CMAKE_COMMAND             = ${CMAKE_COMMAND}" )     # e.g., /usr/bin/cmake
    message( STATUS "CMAKE_VERSION             = ${CMAKE_VERSION}" )     # e.g., 2.8.9
    message( STATUS "CMAKE_BUILD_TOOL          = ${CMAKE_BUILD_TOOL}" )  # e.g., /usr/bin/make
    message( STATUS "STADYN                    = ${STADYN}" )
    message( STATUS "CMAKE_COMPILER_IS_GNUCC   = ${CMAKE_COMPILER_IS_GNUCC}" )
    message( STATUS "CMAKE_CXX_FLAGS           = ${CMAKE_CXX_FLAGS}" )
    message( STATUS "CMAKE_SHARED_LINKER_FLAGS = ${CMAKE_SHARED_LINKER_FLAGS}" )
    message( STATUS "CMAKE_SKIP_BUILD_RPATH    = ${CMAKE_SKIP_BUILD_RPATH}" )
    message( STATUS "CMAKE_INSTALL_RPATH       = ${CMAKE_INSTALL_RPATH}" )
    message( STATUS "CMAKE_BUILD_WITH_INSTALL_RPATH = ${CMAKE_BUILD_WITH_INSTALL_RPATH}" )
    message( STATUS "CMAKE_INSTALL_RPATH_USE_LINK_PATH = ${CMAKE_INSTALL_RPATH_USE_LINK_PATH}" )
    message( STATUS "CMAKE_CURRENT_LIST_DIR   = ${CMAKE_CURRENT_LIST_DIR}" )    # e.g., $HOME/git/gpstk
    message( STATUS "CMAKE_CURRENT_LIST_FILE  = ${CMAKE_CURRENT_LIST_FILE}" )   # e.g., $HOME/git/gpstk/CMakeLists.txt
    message( STATUS "CMAKE_SOURCE_DIR         = ${CMAKE_SOURCE_DIR}" )          # e.g., $HOME/git/gpstk
    message( STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}" )  # e.g., $HOME/git/gpstk
    message( STATUS "CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}" )  # e.g., $HOME/git/gpstk/build
    message( STATUS "PROJECT_NAME             = ${PROJECT_NAME}" )              # e.g., gpstk
    message( STATUS "PROJECT_BINARY_DIR       = ${PROJECT_BINARY_DIR}" )        # e.g., $HOME/git/gpstk/build
    message( STATUS "PROJECT_SOURCE_DIR       = ${PROJECT_SOURCE_DIR}" )        # e.g., $HOME/git/gpstk
    message( STATUS "CMAKE_INSTALL_DIR        = ${CMAKE_INSTALL_DIR}" )         # e.g., /usr
    message( STATUS "CMAKE_INSTALL_PREFIX     = ${CMAKE_INSTALL_PREFIX}" )      # e.g., $HOME/.local/gpstk
endif()

#----------------------------------------
if( DEBUG_VERBOSE )
    get_cmake_property( _variableNames VARIABLES )
    foreach( _variableName ${_variableNames} )
        message( STATUS "---- ${_variableName} = ${${_variableName}}" )
    endforeach()
endif()
        
#----------------------------------------
# Get CMake vars into C++
#----------------------------------------
configure_file( "${PROJECT_SOURCE_DIR}/build_config.h.in" "${PROJECT_BINARY_DIR}/generated/build_config.h" )
include_directories( "${PROJECT_BINARY_DIR}/generated/" ) 
install( FILES "${PROJECT_BINARY_DIR}/generated/build_config.h" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" )


#----------------------------------------
if( ${CMAKE_SYSTEM_NAME} MATCHES "SunOS" )
    set( CPACK_GENERATOR "TGZ" )
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    set( CPACK_GENERATOR "TGZ" )
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    execute_process(COMMAND "/usr/bin/lsb_release" "-is"
                    TIMEOUT 4
                    OUTPUT_VARIABLE LINUX_DISTRO
                    ERROR_QUIET
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (${LINUX_DISTRO} MATCHES "RedHatEnterpriseServer")
        message( STATUS "Detected a Linux Red Hat machine")
        set( CPACK_GENERATOR "DEB;TGZ;RPM")
    elseif (${LINUX_DISTRO} MATCHES "Debian")
        message( STATUS "Detected a Linux Debian machine")
        set( CPACK_GENERATOR "DEB;TGZ" )
    else (${LINUX_DISTRO} MATCHES "RedHatEnterpriseServer")
        message( STATUS "Detected a Linux machine")
        set( CPACK_GENERATOR "DEB;TGZ" )    
    endif()
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
    set( CPACK_GENERATOR "NSIS;ZIP" )
    set( CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.md")
endif()

set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "GPSTk libraries and applications for GNSS processing.") 
set( CPACK_PACKAGE_VENDOR "ARL:UT SGL" )
set( CPACK_PACKAGE_CONTACT "Bryan Parsons" )
set( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md" )
set( CPACK_PACKAGE_VERSION_MAJOR "${GPSTK_VERSION_MAJOR}" )
set( CPACK_PACKAGE_VERSION_MINOR "${GPSTK_VERSION_MINOR}" )
set( CPACK_PACKAGE_VERSION_PATCH "${GPSTK_VERSION_PATCH}" )
set( CPACK_INCLUDE_TOPLEVEL_DIRECTORY "OFF" )
set( CPACK_PACKAGE_INSTALL_DIRECTORY "gpstk")
set( CPACK_TOPLEVEL_TAG "gpstk" ) 

set( CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.13)" )
set( CPACK_DEBIAN_SECTION "stable" )
set( CPACK_DEBIAN_PACKAGE_SECTION "science" )

set( CPACK_SOURCE_IGNORE_FILES "${PROJECT_BINARY_DIR}" "/build-.*/" ".*/[.].*" )
set( CPACK_SOURCE_GENERATOR "TGZ")

include( CPack )
