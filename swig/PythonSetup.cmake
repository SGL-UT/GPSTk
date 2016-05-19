#============================================================
#
# Name    = PythonSetup.cmake
# Purpose = Determine locations of Python library for use by SWIG
# Usage   = add "include( PythonSetup.cmake )" to the appropriate CMakeLists.txt
#
# Scheme  = Top-Level CMakeLists.txt includes this present file
#           First step herein is to look for a CustomPythonSetup.cmake
#           If that fails, this file then tries the "normal" method to find PythonLibs
#           If CMake cannot figure out which Python library to use by that method
#           then modify the template CustomPythonSetup.cmake file to explicitly
#           define the paths to the python library and headers you wish SWIG to use.
#
# Reason  = On systems where the user may have many installations
#           of python, e.g. RedHat or OSX where the system version of Python
#           is an old version not really intended for daily user use, so the user often
#           installs an additional python environment from source or with some package manager.
#           CMake find_package( PythonLibs ) will often stumble on pieces of multiple Python installs
#           in an order that results in mismatching version numbers for the python library
#           and the python include files, and thus cannot build the
#           typemaps for wrapping the C++ code.
#
#============================================================


#------------------------------------------------------------
# If the user provides a custom Python configuration, use it
#------------------------------------------------------------

include( CustomPythonSetup.cmake 
         OPTIONAL
         RESULT_VARIABLE PYTHON_CUSTOM_CONFIG )

#------------------------------------------------------------
# If a user-specified python configuration is not found, let CMake try to find the system python
#------------------------------------------------------------
if( ${PYTHON_CUSTOM_CONFIG} MATCHES "NOTFOUND" )
  find_package( PythonInterp )
  
  # It looks like the find for PythonLibs gets the 'first' Python.h it can find,
  # which does not necessiarly match what the executable found by PythonInterp
  # will be copacetic with. So, we set CMAKE_INCLUDE_PATH to what is returned
  # by the found python-config
  execute_process( COMMAND "${PYTHON_EXECUTABLE}-config" "--includes" OUTPUT_VARIABLE PYTHON_INCLUDES)
  string(REGEX MATCH "^-I(.*) " _python_include ${PYTHON_INCLUDES})
  string(STRIP ${_python_include} _python_include)
  string(SUBSTRING ${_python_include} 2 -1 _python_include) # strip the "-I"
  set(CMAKE_INCLUDE_PATH ${_python_include})
  find_package( PythonLibs ${PYTHON_VERSION_STRING} REQUIRED )
endif()


#------------------------------------------------------------
# Debug messaging
#------------------------------------------------------------
if( DEBUG_SWITCH OR NOT PYTHONLIBS_FOUND)
  message( STATUS "PYTHONINTERP_FOUND        = ${PYTHONINTERP_FOUND}" )
  message( STATUS "PYTHON_EXECUTABLE         = ${PYTHON_EXECUTABLE}" )
  message( STATUS "PYTHON_VERSION_STRING     = ${PYTHON_VERSION_STRING}" )
  message( STATUS "PYTHONLIBS_FOUND          = ${PYTHONLIBS_FOUND}" )
  message( STATUS "PYTHON_LIBRARIES          = ${PYTHON_LIBRARIES}" )
  message( STATUS "PYTHON_INCLUDE_DIR        = ${PYTHON_INCLUDE_DIRS}" )
  message( STATUS "PYTHON_INCLUDE_DIRS       = ${PYTHON_INCLUDE_DIRS}" )
  message( STATUS "PYTHONLIBS_VERSION_STRING = ${PYTHONLIBS_VERSION_STRING}" )
  message( STATUS "PYTHON_INSTALL_PREFIX     = ${PYTHON_INSTALL_PREFIX}" ) 
endif()

#------------------------------------------------------------
# Consistent python library and headers could not be found
#------------------------------------------------------------
if( NOT PYTHONLIBS_FOUND )
  message( STATUS "Cannot find requested version of PYTHONLIBS on your system." )
  message( STATUS "Cannot build swig bindings without the right python libraries." )
  message( STATUS "PYTHON_LIBRARY and PYTHON_INCLUDE_DIR versions must match PYTHON_EXECUTABLE." )
  message( FATAL_ERROR "Cannot find PYTHONLIBS. Cannot proceed. Exiting now!" )
  return()
endif()
