# Testing the swig/python bindings

set (DD ${GPSTK_TEST_DATA_DIR} )
set (OD ${GPSTK_TEST_OUTPUT_DIR} )
set (SD ${PROJECT_BINARY_DIR}/swig)

# Since this requires that there be a valid python runtime, these tests will likely
# be quite different from the C++ library and application tests
add_test(NAME swig_load
  COMMAND ${PYTHON_EXECUTABLE} -c "import gpstk; print 'gpstk imported successfully'"
  WORKING_DIRECTORY ${BD}
  )

add_test(NAME swig_constants
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/swig_constants.py ${SD} ${DD} ${OD}
  WORKING_DIRECTORY ${BD}
  )
