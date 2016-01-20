# Testing the swig/python bindings

set (SD ${GPSTK_TEST_DATA_DIR} )
set (TD ${GPSTK_TEST_OUTPUT_DIR} )
set (BD ${PROJECT_BINARY_DIR}/swig)

# Since this requires that there be a valid python runtime, these tests will likely
# be quite different from the C++ library and application tests
add_test(NAME swig_load
  COMMAND ${PYTHON_EXECUTABLE} -c "import gpstk; print 'gpstk imported successfully'"
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/swig
  )

add_test(NAME swig_constants
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/swig_constants.py ${BD} ${SD} ${TD}
  WORKING_DIRECTORY ${TD}
  )

add_test(NAME swig_obsfilereader
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/swig_obsfilereader.py ${BD} ${SD} ${TD}
  WORKING_DIRECTORY ${TD}
  )
