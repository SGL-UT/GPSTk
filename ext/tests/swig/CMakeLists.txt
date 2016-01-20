# Testing the swig/python bindings

set( SD ${GPSTK_TEST_DATA_DIR} )
set( TD ${GPSTK_TEST_OUTPUT_DIR} )

# Since this requires that there be a valid python runtime, these tests will likely
# be quite different from the C++ library and application tests
add_test(NAME swig_load
  COMMAND ${PYTHON_EXECUTABLE} -c "import gpstk; print 'data_dir is ${SD}';import sys;print 'path:',sys.path; import os; print 'cwd:',os.getcwd()"
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/swig
  )

add_test(NAME swig_constants
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/swig_constants.py
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/swig
  )
