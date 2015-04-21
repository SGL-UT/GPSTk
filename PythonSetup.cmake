#============================================================
#
# Name    = PythonSetup.cmake
# Purpose = Coordinate the Config of Python
# Usage   = add "include( PythonSetup.cmake )" to the appropriate CMakeLists.txt
# Notes   = On systems where the user may have many installations
#           of python, CMake find_package( PythonLibs ) can often 
#           not find matching version numbers for the python library
#           and the python include files, and thus cannot build the
#           typemaps for wrapping the C++ code.
#           One can create a CustomPythonSetup.cmake file to specify explicitly
#           which components of the python install that CMake/SWIG
#           should use.
#============================================================

include( CustomPythonSetup.cmake 
         OPTIONAL
         RESULT_VARIABLE PYTHON_CUSTOM_CONFIG )
if( ${PYTHON_CUSTOM_CONFIG} MATCHES "NOTFOUND" )
    find_package( PythonInterp )
    find_package( PythonLibs ${PYTHON_VERSION} REQUIRED )

    set( PYTHON_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} )
endif()


if( DEBUG_SWITCH )
    message( STATUS "DEBUG: PYTHONINTERP_FOUND        = ${PYTHONINTERP_FOUND}" )
    message( STATUS "DEBUG: PYTHON_EXECUTABLE         = ${PYTHON_EXECUTABLE}" )
    message( STATUS "DEBUG: PYTHON_VERSION_STRING     = ${PYTHON_VERSION_STRING}" )
    message( STATUS "DEBUG: PYTHONLIBS_FOUND          = ${PYTHONLIBS_FOUND}" )
    message( STATUS "DEBUG: PYTHON_LIBRARIES          = ${PYTHON_LIBRARIES}" )
    message( STATUS "DEBUG: PYTHON_INCLUDE_DIRS       = ${PYTHON_INCLUDE_DIRS}" )
    message( STATUS "DEBUG: PYTHONLIBS_VERSION_STRING = ${PYTHONLIBS_VERSION_STRING}" )
    message( STATUS "DEBUG: PYTHON_INSTALL_PREFIX     = ${PYTHON_INSTALL_PREFIX}" ) 
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


#============================================================
# The End
#============================================================
