#============================================================
# Name    = $GPSTK/CMakeLists.txt
# Purpose = Generator of build framework (e.g. Makefiles) for GPSTk
# Notes   = This is the top-level CMake input file
#           Depends on $GPSTK/BuildSetup.cmake
#           Is dependend on by $GPSTK/build.sh
#============================================================

cmake_minimum_required( VERSION 2.8.5 )

project( GPSTK )
set( GPSTK_VERSION_MAJOR "5" )
set( GPSTK_VERSION_MINOR "0" )
set( GPSTK_VERSION_PATCH "0" )
set( GPSTK_VERSION "${GPSTK_VERSION_MAJOR}.${GPSTK_VERSION_MINOR}.${GPSTK_VERSION_PATCH}" )

# Test data file source directory
set( GPSTK_TEST_DATA_DIR ${PROJECT_SOURCE_DIR}/data )
# Test output directory
set( GPSTK_TEST_OUTPUT_DIR ${PROJECT_BINARY_DIR}/Testing/Temporary )

# This sets up variables contining GNU standard installation locations.
include( GNUInstallDirs )

# Set a filename for collecting exported targets.
set( EXPORT_TARGETS_FILENAME "GPSTKTargets" )


option( DEBUG_SWITCH "HELP: DEBUG_SWITCH: Default = OFF, print some CMake variable values to stdout." OFF )
option( DEBUG_VERBOSE "HELP: DEBUG_VERBOSE: Default = OFF, print all CMake variable values." OFF )
option( BUILD_EXT "HELP: BUILD_EXT: SWITCH, Default = OFF, Build the ext library, in addition to the core library." OFF )
option( TEST_SWITCH "HELP: TEST_SWITCH: SWITCH, Default = OFF, Turn on test mode." OFF )
option( COVERAGE_SWITCH "HELP: COVERAGE_SWITCH: SWITCH, Default = OFF, Turn on coverage instrumentation." OFF )
option( BUILD_PYTHON "HELP: BUILD_PYTHON: SWITCH, Default = OFF, Turn on processing of python extension package." OFF )
option( USE_RPATH "HELP: USE_RPATH: SWITCH, Default= ON, Set RPATH in libraries and binaries." ON )
option( PIP_WHEEL_SWITCH "HELP: PIP_WHEEL_SWITCH: SWITCH, Default= OFF, Build a PIP installable wheel." OFF )
option( BUILD_FOR_PACKAGE_SWITCH "HELP: BUILD_FOR_PACKAGE_SWITCH: SWITCH, Default= OFF, Modify python install paths assuming creation of deb/rpm." OFF )
option( PYTHON_USER_INSTALL "HELP: PYTHON_USER_INSTALL: SWITCH, Default= OFF, Install python in user mode." OFF )
option( VERSIONED_HEADER_INSTALL "HELP: VERSIONED_HEADER_INSTALL: SWITCH, Default= OFF, Install header files into maj/min versioned directory." OFF )

if( PYTHON_USER_INSTALL AND !BUILD_PYTHON )
    message( WARNING "Combination of PYTHON_USER_INSTALL=ON and BUILD_PYTHON=OFF is not allowed. " )
    message( WARNING "Setting BUILD_PYTHON = TRUE" )
    set( BUILD_PYTHON TRUE )
endif()

if( PIP_WHEEL_SWITCH AND !BUILD_PYTHON )
    message( WARNING "Combination of PIP_WHEEL_SWITCH=ON and BUILD_PYTHON=OFF is not allowed. Wheels depend on python module." )
    message( WARNING "Setting BUILD_PYTHON = TRUE" )
    set( BUILD_PYTHON TRUE )
endif()

if( BUILD_FOR_PACKAGE_SWITCH AND !BUILD_PYTHON )
    message( WARNING "Combination of BUILD_FOR_PACKAGE_SWITCH=ON and BUILD_PYTHON=OFF is not allowed. Packages depend on python module." )
    message( WARNING "Setting BUILD_PYTHON = TRUE" )
    set( BUILD_PYTHON TRUE )
endif()

if( BUILD_PYTHON AND !BUILD_EXT )
    message( WARNING "Combination of BUILD_PYTHON=ON and BUILD_EXT=OFF is not allowed. Python swig bindings depend on gpstk/ext." )
    message( WARNING "Setting BUILD_EXT = TRUE" )
    set( BUILD_EXT TRUE )
endif()

# GPSTk header file install target (whether it is version dependent changes based on user flag)
if( VERSIONED_HEADER_INSTALL )
    set( GPSTK_INCLUDE_WRAPPER_DIR "gpstk_${GPSTK_VERSION_MAJOR}_${GPSTK_VERSION_MINOR}" )
    set( GPSTK_INCLUDE_TGT "${GPSTK_INCLUDE_WRAPPER_DIR}/gpstk" )
    set( GPSTK_INCLUDE_PARENT "${CMAKE_INSTALL_INCLUDEDIR}/${GPSTK_INCLUDE_WRAPPER_DIR}" )
    set( CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/${GPSTK_INCLUDE_TGT}" )
    message( WARNING "Setting GPSTK_INCLUDE_TGT_DIR = ${CMAKE_INSTALL_INCLUDEDIR}" )
else()
    set( GPSTK_INCLUDE_PARENT "${CMAKE_INSTALL_INCLUDEDIR}" )
    set( CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/gpstk" )
endif()



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

# Remove getopt.h from non-Windows installs
if( NOT WIN32 )
  foreach( _headerFile ${GPSTK_INC_FILES} )
    get_filename_component( _name ${_headerFile} NAME )
    if( ${_name} MATCHES "getopt.h" )
      list( REMOVE_ITEM GPSTK_INC_FILES ${_headerFile} )
    endif()
  endforeach()
endif()

#============================================================
# Define $GPSTK/ext/ additions to Library Target Files
#============================================================

if( BUILD_EXT )
  file( GLOB_RECURSE EXT_SRC_FILES "ext/lib/*.cpp" "ext/lib/*.c" )
  file( GLOB_RECURSE EXT_INC_FILES "ext/lib/*.h" "ext/lib/*.hpp" )

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
add_library( gpstk ${STADYN} ${GPSTK_SRC_FILES} ${GPSTK_INC_FILES} )

# GPSTk library install target
install( TARGETS gpstk DESTINATION "${CMAKE_INSTALL_LIBDIR}" EXPORT "${EXPORT_TARGETS_FILENAME}" )

# GPSTk header file install target (whether it is version dependent changes based on user flag)
install( FILES ${GPSTK_INC_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )


#============================================================
# GPSTK Library parameters
#============================================================

# GPSTk doesn't yet follow semantic versioning. Binary compatibility is only within a specific MAJOR.MINOR version, so
#   SOVERSION has both MAJOR and MINOR for now.
set_target_properties(gpstk PROPERTIES VERSION "${GPSTK_VERSION_MAJOR}.${GPSTK_VERSION_MINOR}.${GPSTK_VERSION_PATCH}"
                                       SOVERSION "${GPSTK_VERSION_MAJOR}.${GPSTK_VERSION_MINOR}")


#============================================================
# Testing
#============================================================

if( TEST_SWITCH )
  enable_testing()
endif( )

#============================================================
# Coverage
#============================================================

if( COVERAGE_SWITCH )
  if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"
    OR ((${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER "4.9.0" ) AND CMAKE_COMPILER_IS_GNUCXX))
    message(STATUS "Enabling address sanitizer for debug build")
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage" )
  endif()
endif( )


#----------------------------------------
# Add sub-directories
#----------------------------------------

add_subdirectory( core )

if( BUILD_EXT )
   add_subdirectory( ext )
   add_subdirectory( examples )
   if( BUILD_PYTHON )
       add_subdirectory( swig )
   endif()
endif()


#----------------------------------------
# Export the project import cmake files.
#----------------------------------------
include( CMakePackageConfigHelpers )
set( INSTALL_CONFIG_DIR ${CMAKE_INSTALL_DATADIR}/cmake/GPSTK)
configure_package_config_file( ${CMAKE_CURRENT_SOURCE_DIR}/GPSTKConfig.cmake.in
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
