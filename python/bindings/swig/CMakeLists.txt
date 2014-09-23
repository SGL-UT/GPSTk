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

find_package(PythonLibs 2 REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}" )

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

if( NOT DEFINED INSTALL_METHOD )
    set( INSTALL_METHOD "4" )
endif()

message( STATUS "INSTALL_METHOD = ${INSTALL_METHOD}" )

# Only install the gpstk module files
if( INSTALL_METHOD STREQUAL "1" )
    set( MODULE_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/gpstk )
    message( STATUS "SWIG module install path = ${MODULE_INSTALL_PATH}" )
    install( TARGETS "_${SWIG_MODULE}" DESTINATION gpstk )
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${SWIG_MODULE}.py DESTINATION gpstk )
    install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py DESTINATION gpstk )
endif()

# Install the gpstk package file tree, then install the gpstk extension module into the package
if( INSTALL_METHOD STREQUAL "2" )
    set( INSTALL_PACKAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/install_package/gpstk )
    message( STATUS "SWIG install package root = ${INSTALL_PACKAGE_PATH}" )
    install( DIRECTORY ${INSTALL_PACKAGE_PATH} DESTINATION ${CMAKE_INSTALL_PREFIX} )
    install( TARGETS "_${SWIG_MODULE}" DESTINATION gpstk )
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${SWIG_MODULE}.py DESTINATION gpstk )
endif()

# Install the gpstk module into the package file tree, and then install the package file tree 
if( INSTALL_METHOD STREQUAL "3" )
    set( INSTALL_PACKAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/install_package )
    message( STATUS "SWIG install package root = ${INSTALL_PACKAGE_PATH}" )
    install( TARGETS "_${SWIG_MODULE}" DESTINATION ${INSTALL_PACKAGE_PATH}/gpstk )
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${SWIG_MODULE}.py DESTINATION ${INSTALL_PACKAGE_PATH}/gpstk )
    install( DIRECTORY ${INSTALL_PACKAGE_PATH} DESTINATION ${CMAKE_INSTALL_PREFIX} )
endif()

# Install the gpstk module into the package file tree, and then STOP and let user run the setup.py script manually
if( INSTALL_METHOD STREQUAL "4" )
    set( INSTALL_PACKAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/install_package )
    message( STATUS "SWIG install package root = ${INSTALL_PACKAGE_PATH}" )
    install( TARGETS "_${SWIG_MODULE}" DESTINATION ${INSTALL_PACKAGE_PATH}/gpstk )
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${SWIG_MODULE}.py DESTINATION ${INSTALL_PACKAGE_PATH}/gpstk )
endif()
	
# Install the gpstk module into the package file tree, and then have CMake create a make target to call the setup.py script
if( INSTALL_METHOD STREQUAL "5" )

  # Create targets for installing the python extension module into the package file tree
  set( INSTALL_PACKAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/install_package )
  message( STATUS "SWIG install package root = ${INSTALL_PACKAGE_PATH}" )
  install( TARGETS "_${SWIG_MODULE}" DESTINATION ${INSTALL_PACKAGE_PATH}/gpstk )
  install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${SWIG_MODULE}.py DESTINATION ${INSTALL_PACKAGE_PATH}/gpstk )

  # Install the package file tree using distutils
  find_program(PYTHON "python")
  if( PYTHON )
    message( STATUS "find_program returned this PYTHON: ${PYTHON}" )

    # Define filenames of stuff we need to build/install this module
    set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
    set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/install_package/setup.py")
    set(DEPS        "${CMAKE_CURRENT_SOURCE_DIR}/install_package/gpstk/__init__.py")
    set(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build/timestamp")

    # CMake congifure_file() is to copy and modify files as part of creating the makefiles.
	# With the configure_file() command CMake allows 
	#   (1) copying a file from the source directory to the binary directory,
	#   (2) replacement of CMake variables within the text by encasing the variable name in @@ (e.g. @variable_name@). 
	# Here's how to use it:
	#   - Create a regular file and rename it; the renamed file will be a "template" for recreating the original and variations.
	#       e.g. setup.py is renamed to setup.py.in
	#   - In the template (renamed) file, replace a variable the CMake variable enclosed in a pair of "@",
	#       e.g. "version = @CMAKE_MY_PACKAGE_VERSION@" takes the value of the CMake variable "${CMAKE_MY_PACKAGE_VERSION}"
	#   - Use the configure_file() directive to copy the setup.py.in file to setup.py 
	#     and use configure_file() input arges to tell it how to 
	#     replace the CMake style variables with actual values.
	#     e.g. configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in" "${BINARY_BUILD_DIR}/setup.py" @ONLY)
	#   - Final result is a setup.py the is customize based on variables from CMake.
	# In our case, we don't change anything, but add this directive as a hook for the future.
    configure_file(${SETUP_PY_IN} ${SETUP_PY})

    # use a custom command to create proper dependencies in the make targets
    add_custom_command( OUTPUT ${OUTPUT}
                        COMMAND ${PYTHON} ${SETUP_PY} build
                        COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}
                        DEPENDS ${DEPS} )
    add_custom_target( target ALL DEPENDS ${OUTPUT} )

    # default command to run should be "$ python setup.py install --prefix=~/.local"
    install( CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install -f --prefix=${CMAKE_INSTALL_PREFIX})" )

  endif()

endif()
	
	
	
	
#------------------------------------------------------------
# The End
#------------------------------------------------------------
