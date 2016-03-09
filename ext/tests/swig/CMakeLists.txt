# Testing the swig/python bindings
#
# Since this requires that there be a valid python runtime, these tests will likely
# be quite different from the C++ library and application tests

set(swig_dir ${PROJECT_BINARY_DIR}/swig)

add_test(NAME swig_test_import
    COMMAND ${PYTHON_EXECUTABLE} -c "import gpstk; print 'gpstk imported successfully'"
    WORKING_DIRECTORY ${swig_dir}
    )
set_property(TEST swig_test_import PROPERTY LABELS swig)

add_test(NAME swig_test_constants
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_constants.py ${swig_dir}
    )
set_property(TEST swig_test_constants PROPERTY LABELS swig constants)

add_test(NAME swig_test_rinex3
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_rinex3.py ${swig_dir}
    -i ${GPSTK_TEST_DATA_DIR} -o ${GPSTK_TEST_OUTPUT_DIR}
  )
set_property(TEST swig_test_rinex3 PROPERTY LABELS swig RINEX3)

add_test(NAME swig_test_time
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_time.py ${swig_dir}
    -i ${GPSTK_TEST_DATA_DIR} -o ${GPSTK_TEST_OUTPUT_DIR}
    )
set_property(TEST swig_test_time PROPERTY LABELS swig Time CommonTime)
