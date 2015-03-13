#----------------------------------------
# CMake Input: $GPSTK/swig/CMakeLists.txt
#----------------------------------------

cmake_minimum_required( VERSION 2.8.4  )

message( STATUS "Inside gpstk/swig/CMakeLists.txt, PYTHON_INSTALL_PREFIX = ${PYTHON_INSTALL_PREFIX}" )

# The CMake command-line tool displays STATUS messages
# on stdout and all other message types on stderr.
#   set( CMAKE_PREFIX_PATH "" )
#   list( APPEND CMAKE_PREFIX_PATH /opt/python/2.7.6 )
#   message( STATUS "DEBUG: SWIG: CMAKE_PREFIX_PATH  = ${CMAKE_PREFIX_PATH}" )

#------------------------------------------------------------
# Find python executable
#------------------------------------------------------------

# If user does not pass in the full-path to the python executable,
# then figure it out by running python.
if( NOT PYTHON_EXECUTABLE )
    message( WARNING  "PYTHON_EXECUTABLE is undefined. Will attempt find a python instance. Next time, pass in a full path to cmake using, e.g., cmake -DPYTHON_EXECUTABLE=/usr/bin/python2.7" )
    execute_process( COMMAND python -c
		             "import sys; print sys.executable"
                     OUTPUT_VARIABLE PYTHON_EXECUTABLE
                     OUTPUT_STRIP_TRAILING_WHITESPACE
                    )
endif()

# If PYTHON_EXECUTABLE is still empty, something is severely wrong,
#  perhaps python is not installed on this system?
if( NOT PYTHON_EXECUTABLE )
	message( FATAL_ERROR "PYTHON_EXECUTABLE: Unable to determine or use PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE}" )
	message( FATAL_ERROR "PYTHON_EXECUTABLE: Exitting now!" )
    return()
endif()

#------------------------------------------------------------
# Get python version number
#------------------------------------------------------------

# Now figure out the version of the python executable
# so we can go look for a matching python library to link against.
execute_process( COMMAND ${PYTHON_EXECUTABLE} -c
	             "import sys; print sys.version.split()[0]"
                 OUTPUT_VARIABLE PYTHON_version_output
                 OUTPUT_STRIP_TRAILING_WHITESPACE
                )

# Set the version number and then test the value to make sure it's not empty
set( PYTHON_VERSION ${PYTHON_version_output} CACHE STRING "Python version" )
if( NOT PYTHON_VERSION STREQUAL "" )
	set( PYTHON_FOUND 1 )
    set( PYTHON ${PYTHON_EXECUTABLE} )
else()
    message( FATAL_ERROR "Cannot determine version number for PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE}" )
    return()
endif()

# Test the version number to make sure it's above the minimum needed
if( PYTHON_VERSION VERSION_LESS 2.7 )
    set( PYTHON_FOUND 0 )
    set( PYTHON 0 )
    set( PYTHONLIBS_FOUND 0 )
    message( FATAL_ERROR "Python version [ ${PYTHON_VERSION} ] is >= the required 2.7, which is needed because argparse is used in the gpstk python module. Exit now!" )
    return()
endif()

#------------------------------------------------------------
# Find matching version of python library
#------------------------------------------------------------

find_package( PythonLibs ${PYTHON_VERSION} REQUIRED )

if( DEBUG_SWITCH )
    message( STATUS "DEBUG: SWIG: PYTHON_EXECUTABLE        = ${PYTHON_EXECUTABLE}" )
    message( STATUS "DEBUG: SWIG: PYTHON_FOUND             = ${PYTHON_FOUND}" )
    message( STATUS "DEBUG: SWIG: PYTHONLIBS_FOUND         = ${PYTHONLIBS_FOUND}" )
    message( STATUS "DEBUG: SWIG: PYTHON                   = ${PYTHON}" )
    message( STATUS "DEBUG: SWIG: PYTHON_VERSION           = ${PYTHON_VERSION}" )
    message( STATUS "DEBUG: SWIG: PYTHON_LIBRARIES         = ${PYTHON_LIBRARIES}" )
    message( STATUS "DEBUG: SWIG: PYTHON_LIBRARY           = ${PYTHON_LIBRARY}" )
    message( STATUS "DEBUG: SWIG: PYTHON_INCLUDE_DIR       = ${PYTHON_INCLUDE_DIR}" )
endif()

if( NOT PYTHONLIBS_FOUND )
    message( STATUS "Cannot find requested version of PYTHONLIBS on your system." )
    message( STATUS "Cannot build swig bindings without the right python libraries." )
    message( STATUS "PYTHON_LIBRARY and PYTHON_INCLUDE_DIR versions must match PYTHON_EXECUTABLE." )
    message( STATUS "DEBUG: SWIG: PYTHON_EXECUTABLE        = ${PYTHON_EXECUTABLE}" )
    message( STATUS "DEBUG: SWIG: PYTHON_FOUND             = ${PYTHON_FOUND}" )
    message( STATUS "DEBUG: SWIG: PYTHONLIBS_FOUND         = ${PYTHONLIBS_FOUND}" )
    message( STATUS "DEBUG: SWIG: PYTHON                   = ${PYTHON}" )
    message( STATUS "DEBUG: SWIG: PYTHON_VERSION           = ${PYTHON_VERSION}" )
    message( STATUS "DEBUG: SWIG: PYTHON_LIBRARIES         = ${PYTHON_LIBRARIES}" )
    message( STATUS "DEBUG: SWIG: PYTHON_LIBRARY           = ${PYTHON_LIBRARY}" )
    message( STATUS "DEBUG: SWIG: PYTHON_INCLUDE_DIR       = ${PYTHON_INCLUDE_DIR}" )
    message( FATAL_ERROR "Cannot find PYTHONLIBS. Cannot proceed. Exiting now!" )
    return()
else()
    message( STATUS "PYTHONLIBS Version requested was found. Yay for you!" )
endif()

#------------------------------------------------------------
# Python Library includes
#------------------------------------------------------------

include_directories( ${PYTHON_INCLUDE_PATH} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

#------------------------------------------------------------
# Find system SWIG package
#------------------------------------------------------------

find_package( SWIG )

if( NOT SWIG_FOUND )
    message( FATAL_ERROR "SWIG: System install of swig not found. Cannot build swig bindings." )
    return()
endif()

if( NOT SWIG_EXECUTABLE )
    message( FATAL_ERROR "SWIG: System install of swig not found. Cannot build swig bindings." )
    return()
endif()

include( ${SWIG_USE_FILE} )

if( DEBUG_SWITCH )
    message( STATUS "DEBUG: SWIG: SWIG_EXECUTABLE          = ${SWIG_EXECUTABLE}" )
    message( STATUS "DEBUG: SWIG: SWIG_USE_FILE            = ${SWIG_USE_FILE}" )
endif()

#----------------------------------------
# Compiler warning disables
#----------------------------------------
# CMAKE_SWIG_FLAGS
#     362: operator= ignored
#     383: operator++ ignored
#     384: operator-- ignored
#     389: operator[] ignored
#     503: can't wrap 'identifer' unless renamed to a valid identifier (e.g. operator<<)

set( CMAKE_SWIG_FLAGS "-w362,383,384,389,503" )

#----------------------------------------
# Compiler CXX flags
#----------------------------------------

set( CMAKE_CXX_FLAGS "-std=c++11" )

set_source_files_properties( gpstk.i PROPERTIES CPLUSPLUS ON ) # why not also set properties for all other files.i ?!?!


#----------------------------------------
# Debug
#----------------------------------------
# The CMake command-line tool, message( STATUS ) displays
# messages on stdout and all other message types on stderr.

if( DEBUG_SWITCH )
    message( STATUS "DEBUG: SWIG: DEBUG: Before swig module is set -----------------------------------------" )
    message( STATUS "DEBUG: SWIG: CMAKE_CURRENT_LIST_DIR   = ${CMAKE_CURRENT_LIST_DIR}" )    # e.g., $HOME/git/gpstk/swig
    message( STATUS "DEBUG: SWIG: CMAKE_CURRENT_LIST_FILE  = ${CMAKE_CURRENT_LIST_FILE}" )   # e.g., $HOME/git/gpstk/swig/CMakeLists.txt
    message( STATUS "DEBUG: SWIG: CMAKE_SOURCE_DIR         = ${CMAKE_SOURCE_DIR}" )          # e.g., $HOME/git/gpstk
    message( STATUS "DEBUG: SWIG: CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}" )  # e.g., $HOME/git/gpstk/swig
    message( STATUS "DEBUG: SWIG: CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}" )  # e.g., $HOME/git/gpstk/build/swig

    message( STATUS "DEBUG: SWIG: PROJECT_NAME             = ${PROJECT_NAME}" )              # e.g., gpstk
    message( STATUS "DEBUG: SWIG: PROJECT_BINARY_DIR       = ${PROJECT_BINARY_DIR}" )        # e.g., $HOME/git/gpstk/build
    message( STATUS "DEBUG: SWIG: PROJECT_SOURCE_DIR       = ${PROJECT_SOURCE_DIR}" )        # e.g., $HOME/git/gpstk

    message( STATUS "DEBUG: SWIG: CMAKE_INSTALL_PREFIX     = ${CMAKE_INSTALL_PREFIX}" )      # e.g., $HOME/.local/gpstk
    message( STATUS "DEBUG: SWIG: PYTHON_INSTALL_PREFIX    = ${PYTHON_INSTALL_PREFIX}" )     # e.g., $HOME/.local
    message( STATUS "DEBUG: SWIG: CMAKE_INSTALL_DIR        = ${CMAKE_INSTALL_DIR}" )         # e.g., [empty]
endif()

#----------------------------------------
# Swig module build target
#----------------------------------------
# Note: the cmake swig module doesn't seem to know
#       how to track dependancy on other .i files...
#       so if the other .i files are changed,
#       you must rerun cmake, not just make.
#
# Note: list of all headers files are included in
#       gpstk_swig.hpp which is included in gpstk.i

# make sure the swig build target outputs files to the build file tree
# set( CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/install_package/${SWIG_MODULE} )

set( SWIG_MODULE "gpstk" )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/src )

swig_add_module( ${SWIG_MODULE} python gpstk.i ) # why not also list all the other files.i here?!
swig_link_libraries( ${SWIG_MODULE} gpstk )
swig_link_libraries( ${SWIG_MODULE} ${PYTHON_LIBRARIES} )

#----------------------------------------
# Copy python extension module output into package build tree
#----------------------------------------

# Install the gpstk module into the package file tree
set( INSTALL_PACKAGE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/install_package )
set( INSTALL_PACKAGE_PATH ${CMAKE_CURRENT_BINARY_DIR}/install_package )

if( DEBUG_SWITCH )
    message( STATUS "DEBUG: SWIG: DEBUG: After swig module is set -----------------------------------------" )
    message( STATUS "DEBUG: SWIG: EXECUTABLE_OUTPUT_PATH   = ${EXECUTABLE_OUTPUT_PATH}" )
    message( STATUS "DEBUG: SWIG: WORKING_DIRECTORY        = ${WORKING_DIRECTORY}" )
    message( STATUS "DEBUG: SWIG: CMAKE_SOURCE_DIR         = ${CMAKE_CURRENT_DIR}" )
    message( STATUS "DEBUG: SWIG: CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}" )
    message( STATUS "DEBUG: SWIG: CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}" )
    message( STATUS "DEBUG: SWIG: CMAKE_INSTALL_PREFIX     = ${CMAKE_INSTALL_PREFIX}" )
    message( STATUS "DEBUG: SWIG: PYTHON_INSTALL_PREFIX    = ${PYTHON_INSTALL_PREFIX}" )     # e.g., $HOME/.local
    message( STATUS "DEBUG: SWIG: CMAKE_SWIG_OUTDIR        = ${CMAKE_SWIG_OUTDIR}" )
    message( STATUS "DEBUG: SWIG: INSTALL_PACKAGE_SOURCE   = ${INSTALL_PACKAGE_SOURCE}" )
    message( STATUS "DEBUG: SWIG: INSTALL_PACKAGE_PATH     = ${INSTALL_PACKAGE_PATH}" )
endif()

#----------------------------------------
# Add custom commands to the _gpstk.so build target
#----------------------------------------
#    ...to relocate the output from teh swig build target to
#    the correct directory within the python package file tree
#        CMake documentation claims that all SWIG target output
#        will be written to $CMAKE_SWIG_OUTDIR, but
#        testing shows that this ONLY works for the gpstk.py
#        and does NOT work for the _gpstk.so
#----------------------------------------
add_custom_command( TARGET "_${SWIG_MODULE}" POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "_${SWIG_MODULE}.so" ${INSTALL_PACKAGE_PATH}/${SWIG_MODULE} )
add_custom_command( TARGET "_${SWIG_MODULE}" POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${SWIG_MODULE}.py" ${INSTALL_PACKAGE_PATH}/${SWIG_MODULE} )

# CONFIGURATION:
# It copies entire install_package file tree
# from it's original src root to the build root
file( COPY ${INSTALL_PACKAGE_SOURCE}/ DESTINATION ${INSTALL_PACKAGE_PATH} PATTERN "*" )

# CONFIGURATION: Copy meta files.txt
set( METAFILES "" ) # for a single set() command, just ";" delimiter between entries in the list
list( APPEND METAFILES "${CMAKE_SOURCE_DIR}/AUTHORS.txt" )
list( APPEND METAFILES "${CMAKE_SOURCE_DIR}/COPYING.txt" )
list( APPEND METAFILES "${CMAKE_SOURCE_DIR}/COPYING.LESSER.txt" )
list( APPEND METAFILES "${CMAKE_SOURCE_DIR}/ChangeLog.txt" )
list( APPEND METAFILES "${CMAKE_SOURCE_DIR}/INSTALL.txt" )
list( APPEND METAFILES "${CMAKE_SOURCE_DIR}/NEWS.txt" )
list( APPEND METAFILES "${CMAKE_SOURCE_DIR}/README.txt" )
file( COPY ${METAFILES} DESTINATION ${INSTALL_PACKAGE_PATH} )

#--------------------------------------------
# Initialize a working space to build the doxygen/sphinx docs
#--------------------------------------------

set( SWIG_DOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/doc )
file( MAKE_DIRECTORY ${SWIG_DOC_DIR} )
file( WRITE ${SWIG_DOC_DIR}/doc.i "#dummy file. Is replaced when docs are generated for bindings." )
include_directories( ${SWIG_DOC_DIR} )

#----------------------------------------
# Install package tree
#----------------------------------------

# Provide ability to use CMake variables to define some values within the setup.py script
set( PYTHON_PACKAGE_VERSION "${gpstk_VERSION_MAJOR}.${gpstk_VERSION_MINOR}" )
set( SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in" )
set( SETUP_PY    "${INSTALL_PACKAGE_PATH}/setup.py" )
configure_file( ${SETUP_PY_IN} ${SETUP_PY} )

#------------------------------------------------------------
# Install python package
#------------------------------------------------------------

# Note: For setup.py install options, see https://docs.python.org/2/install/
# Note: The two words "WORKING_DIRECTORY ${INSTALL_PACKAGE_PATH}" are a key-value pair
#       This sets the value of WORKING_DIRECTORY equal to ${INSTALL_PACKAGE_PATH}, i.e.
#       the command being run treats ${INSTALL_PACKAGE_PATH} as the current working directory


if( NOT PYTHON_INSTALL_PREFIX )
    message( WARNING "PYTHON_INSTALL_PREFIX: Install prefix for python package was not set by user." )
    set( PYTHON_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" )
    message( WARNING "PYTHON_INSTALL_PREFIX: Using CMAKE_INSTALL_PREFIX as the default, so that PYTHON_INSTALL_PREFIX = ${PYTHON_INSTALL_PREFIX}" )
endif()

install( CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install --prefix=${PYTHON_INSTALL_PREFIX} WORKING_DIRECTORY ${INSTALL_PACKAGE_PATH})" )

#------------------------------------------------------------
# These files aren't required to use the bindings but are
# required to make a swig module that depends upon these bindings
#------------------------------------------------------------

file( GLOB swig_includes ${CMAKE_CURRENT_SOURCE_DIR}/src/* )
install( FILES ${swig_includes} ${CMAKE_CURRENT_SOURCE_DIR}/gpstk.i DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" )

#------------------------------------------------------------
# The End
#------------------------------------------------------------
