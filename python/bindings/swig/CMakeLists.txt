cmake_minimum_required(VERSION 2.8)

#------------------------------------------------------------
# CMake Scope
#------------------------------------------------------------
# Please note that this file does not use any of the CMakeLists
# files in the GPSTK /dev directory. This CMakeLists file is a
# separate project that uses a CMake Find module to locate the
# GPSTK install and source files

project( pygpstk )

#------------------------------------------------------------
# CMAKE_INSTALL_PREFIX
#------------------------------------------------------------
# The default CMAKE_INSTALL_PREFIX is inappropriate for python modules so this variable needs
# to be overridden. To install in the user's directory:
#   cmake -DCMAKE_INSTALL_PREFIX=$(python -m site --user-site) ..
# Since the default is never correct, at least put in something reasonable

if ("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local")
  # set a variable for the PYTHON_SITE_PACKAGES but call it CMAKE_INSTALL_PREFIX
  execute_process(COMMAND python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()"
    OUTPUT_VARIABLE CMAKE_INSTALL_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

#------------------------------------------------------------
# SWIG and PythonLibs packages
#------------------------------------------------------------

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
set(CMAKE_SWIG_FLAGS "")

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "Building against python include path: ${PYTHON_INCLUDE_PATH}")
message(STATUS "Building against python library: ${PYTHON_LIBRARIES}")

#------------------------------------------------------------
# CMake Find module used to find the installed GPSTK
#------------------------------------------------------------
# User must define GPSTK install path:
#     Method 1:  $ export gpstk=/home/vestuto/git/gpstk/dev/install; cmake;
#     Method 2:  $ cmake -DGPSTK_INSTALL=/home/vestuto/git/gpstk/dev/install
#     Method 3:  set( GPSTK_INSTALL "/home/vestuto/git/gpstk/dev/install" )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules )
find_package( GPSTK )
if( NOT GPSTK_FOUND )
  message(WARNING "Cannot find GPSTK library install. Exitting." )
  return()
else()
  message( STATUS "Found GPSTK library install"  )
  message( STATUS "GPSTK_INCLUDE_DIRS = ${GPSTK_INCLUDE_DIRS}"  )
  message( STATUS "GPSTK_LIBRARY_DIRS = ${GPSTK_LIBRARY_DIRS}"  )
endif()
include_directories( ${GPSTK_INCLUDE_DIRS} )
link_directories( ${GPSTK_LIBRARY_DIRS} )

#----------------------------------------
# Compiler warning disables
#----------------------------------------
# CMAKE_SWIG_FLAGS
#     362: operator= ignored
#     383: operator++ ignored
#     384: operator-- ignored
#     389: operator[] ignored
#     503: can't wrap 'identifer' unless renamed to a valid identifier (e.g. operator<<)

set(CMAKE_SWIG_FLAGS "-w362,383,384,389,503")

#----------------------------------------
# Compiler CXX flags
#----------------------------------------

set(CMAKE_CXX_FLAGS "-std=c++11")
set_source_files_properties(gpstk.i PROPERTIES CPLUSPLUS ON)
 
#----------------------------------------
# Swig module target
#----------------------------------------
# Note: the cmake swig module doesn't seem to know
#       how to track dependancy on other .i files...
#       so if the other .i files are changed, do a make clean....
#
# Note: list of all headers files are included in 
#       gpstk_swig.hpp which is included in gpstk.i

set( SWIG_MODULE "gpstk" )
swig_add_module( ${SWIG_MODULE} python gpstk.i )
swig_link_libraries( ${SWIG_MODULE} gpstk )
swig_link_libraries( ${SWIG_MODULE} ${PYTHON_LIBRARIES} )

#----------------------------------------
# Install with CMake install() directive
#----------------------------------------

message( STATUS "SWIG module install path = ${CMAKE_INSTALL_PREFIX}" )
install( TARGETS "_${SWIG_MODULE}" DESTINATION ${CMAKE_INSTALL_PREFIX} )
install( FILES "${CMAKE_CURRENT_BINARY_DIR}/${SWIG_MODULE}.py" DESTINATION ${CMAKE_INSTALL_PREFIX} )

#------------------------------------------------------------
# The End
#------------------------------------------------------------
