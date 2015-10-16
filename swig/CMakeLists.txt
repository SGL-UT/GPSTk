#----------------------------------------
# gpstk/swig/CMakeLists.txt
#----------------------------------------

cmake_minimum_required( VERSION 2.8.4  )

include_directories( ${PYTHON_INCLUDE_DIRS} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

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

set( CMAKE_CXX_FLAGS "-std=c++11" )

set_source_files_properties( gpstk.i PROPERTIES CPLUSPLUS ON ) # why not also set properties for all other files.i ?!?!

if( NOT PYTHON_INSTALL_PREFIX )
    message( STATUS "PYTHON_INSTALL_PREFIX: Install prefix for python package was not set by user." )
    set( PYTHON_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" )
    message( STATUS "PYTHON_INSTALL_PREFIX: Using CMAKE_INSTALL_PREFIX as the default, so that PYTHON_INSTALL_PREFIX = ${PYTHON_INSTALL_PREFIX}" )
endif()

if( DEBUG_SWITCH )
    message( STATUS "SWIG_EXECUTABLE          = ${SWIG_EXECUTABLE}" )
    message( STATUS "SWIG_USE_FILE            = ${SWIG_USE_FILE}" )
    message( STATUS "CMAKE_CURRENT_LIST_DIR   = ${CMAKE_CURRENT_LIST_DIR}" )    # e.g., $HOME/git/gpstk/swig
    message( STATUS "CMAKE_CURRENT_LIST_FILE  = ${CMAKE_CURRENT_LIST_FILE}" )   # e.g., $HOME/git/gpstk/swig/CMakeLists.txt
    message( STATUS "CMAKE_SOURCE_DIR         = ${CMAKE_SOURCE_DIR}" )          # e.g., $HOME/git/gpstk
    message( STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}" )  # e.g., $HOME/git/gpstk/swig
    message( STATUS "CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}" )  # e.g., $HOME/git/gpstk/build/swig
    message( STATUS "PYTHON_INSTALL_PREFIX    = ${PYTHON_INSTALL_PREFIX}" )     # e.g., $HOME/.local
    message( STATUS "SWIG_DOC_DIR             = ${SWIG_DOC_DIR}")
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

# Install the gpstk module into the package file tree
set( INSTALL_PACKAGE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/install_package )
set( INSTALL_PACKAGE_PATH ${CMAKE_CURRENT_BINARY_DIR}/install_package )

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
list( APPEND METAFILES "${CMAKE_SOURCE_DIR}/AUTHORS.md" )
list( APPEND METAFILES "${CMAKE_SOURCE_DIR}/COPYING.md" )
list( APPEND METAFILES "${CMAKE_SOURCE_DIR}/COPYING.LESSER.md" )
list( APPEND METAFILES "${CMAKE_SOURCE_DIR}/ChangeLog.md" )
list( APPEND METAFILES "${CMAKE_SOURCE_DIR}/INSTALL.md" )
list( APPEND METAFILES "${CMAKE_SOURCE_DIR}/NEWS.md" )
list( APPEND METAFILES "${CMAKE_SOURCE_DIR}/README.md" )
file( COPY ${METAFILES} DESTINATION ${INSTALL_PACKAGE_PATH} )

#--------------------------------------------
# Initialize a working space to build the doxygen/sphinx docs
#--------------------------------------------

set( SWIG_DOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/doc )
include_directories( ${SWIG_DOC_DIR} )
file( MAKE_DIRECTORY ${SWIG_DOC_DIR} )
file( WRITE ${SWIG_DOC_DIR}/doc.i "// dummy file. Is replaced when docs are generated for bindings." )

# Provide ability to use CMake variables to define some values within the setup.py script
set( SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in" )
set( SETUP_PY    "${INSTALL_PACKAGE_PATH}/setup.py" )
configure_file( ${SETUP_PY_IN} ${SETUP_PY} )

# Note: For setup.py install options, see https://docs.python.org/2/install/
install( CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${SETUP_PY} install --prefix=${PYTHON_INSTALL_PREFIX} WORKING_DIRECTORY ${INSTALL_PACKAGE_PATH})" )

#------------------------------------------------------------
# These files aren't required to use the bindings but are
# required to make a swig module that depends upon these bindings
#------------------------------------------------------------

file( GLOB swig_includes ${CMAKE_CURRENT_SOURCE_DIR}/src/* )
install( FILES ${swig_includes} ${CMAKE_CURRENT_SOURCE_DIR}/gpstk.i DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" )
