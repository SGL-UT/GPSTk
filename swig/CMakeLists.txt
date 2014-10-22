cmake_minimum_required(VERSION 2.8)

#------------------------------------------------------------
# Test for python before doing anything
#------------------------------------------------------------
find_program(PYTHON "python")
if( PYTHON )
  message(STATUS "Using ${PYTHON} for swig bindings" )
else()
  message(STATUS "Can not find executable. Not building swig bindings." )
  return()
endif()

#------------------------------------------------------------
# SWIG and PythonLibs packages
#------------------------------------------------------------

find_package(SWIG)
if(NOT SWIG_EXECUTABLE)
  message(STATUS "Swig not found. Not building swig bindings.")
  return()
endif(NOT SWIG_EXECUTABLE)
include(${SWIG_USE_FILE})
message(STATUS "SWIG_EXECUTABLE: ${SWIG_EXECUTABLE}" )

find_package(PythonLibs 2 REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
swig_add_module(${SWIG_MODULE} python gpstk.i )
swig_link_libraries( ${SWIG_MODULE} gpstk )
swig_link_libraries( ${SWIG_MODULE} ${PYTHON_LIBRARIES} )

#----------------------------------------
# Install python extension module into package tree
#----------------------------------------

# Install the gpstk module into the package file tree
set(INSTALL_PACKAGE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/install_package)
set(INSTALL_PACKAGE_PATH ${CMAKE_CURRENT_BINARY_DIR}/install_package)
message(STATUS "SWIG install package dest = ${INSTALL_PACKAGE_PATH}" )
install(TARGETS "_${SWIG_MODULE}" DESTINATION ${INSTALL_PACKAGE_PATH}/${SWIG_MODULE} )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${SWIG_MODULE}.py DESTINATION ${INSTALL_PACKAGE_PATH}/${SWIG_MODULE} )
file(COPY ${INSTALL_PACKAGE_SOURCE}/ DESTINATION ${INSTALL_PACKAGE_PATH} PATTERN "*")
set(METAFILES "${CMAKE_SOURCE_DIR}/README.txt;${CMAKE_SOURCE_DIR}/AUTHORS.txt;${CMAKE_SOURCE_DIR}/COPYING.txt;${CMAKE_SOURCE_DIR}/COPYING.LESSER.txt;${CMAKE_SOURCE_DIR}/ChangeLog.txt;${CMAKE_SOURCE_DIR}/INSTALL.txt;${CMAKE_SOURCE_DIR}/NEWS.txt")
file(COPY ${METAFILES} DESTINATION ${INSTALL_PACKAGE_PATH} )

#--------------------------------------------
# Initialize a working space to build the doxygen/sphinx docs
#--------------------------------------------
set(SWIG_DOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/doc)
file(MAKE_DIRECTORY ${SWIG_DOC_DIR})
file(WRITE ${SWIG_DOC_DIR}/doc.i "#dummy file. Is replaced when docs are generated for bindings.")
include_directories(${SWIG_DOC_DIR})

#----------------------------------------
# Install package tree 
#----------------------------------------

# Provide ability to use CMake variables to define some values within the setup.py script
set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in" )
set(SETUP_PY    "${INSTALL_PACKAGE_PATH}/setup.py" )
configure_file( ${SETUP_PY_IN} ${SETUP_PY} )

# To install in the user's directory, define PYTHON_USER_INSTALL:
if(PYTHON_USER_INSTALL)
  message(STATUS "Python bindings will be installed for user only.")
  install(CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install -f --user WORKING_DIRECTORY ${INSTALL_PACKAGE_PATH})" )
else()
  message(STATUS "Python bindings will be installed in system directory.")
  install(CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install -f  WORKING_DIRECTORY ${INSTALL_PACKAGE_PATH})" )
endif(PYTHON_USER_INSTALL)


#------------------------------------------------------------
# These files aren't required to use the bindings but are requried to make a swig module that
# depends upon these bindings
#------------------------------------------------------------
file(GLOB swig_includes ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
install(FILES ${swig_includes} ${CMAKE_CURRENT_SOURCE_DIR}/gpstk.i DESTINATION include )


#------------------------------------------------------------
# The End
#------------------------------------------------------------
