# Testing the swig/python bindings
#
# Since this requires that there be a valid python runtime, these tests are
# be quite different from the C++ library and application tests

add_test(NAME swig_test_import
  COMMAND ${PYTHON_EXECUTABLE} -c "import gpstk; print 'gpstk imported successfully'"
  WORKING_DIRECTORY ${MODULE_PATH}
  )
set_property(TEST swig_test_import PROPERTY LABELS swig)


set(td  ${MODULE_PATH}/tests)

add_test(NAME swig_test_constants
  COMMAND ${PYTHON_EXECUTABLE} test_constants.py
  WORKING_DIRECTORY ${td}
  )
set_property(TEST swig_test_constants PROPERTY LABELS swig constants)

add_test(NAME swig_test_rinex3
  COMMAND ${PYTHON_EXECUTABLE} test_rinex3.py -i ${GPSTK_TEST_DATA_DIR} -o ${GPSTK_TEST_OUTPUT_DIR}
  WORKING_DIRECTORY ${td}
  )
set_property(TEST swig_test_rinex3 PROPERTY LABELS swig RINEX3)

add_test(NAME swig_test_time
  COMMAND ${PYTHON_EXECUTABLE} test_time.py -i ${GPSTK_TEST_DATA_DIR} -o ${GPSTK_TEST_OUTPUT_DIR}
  WORKING_DIRECTORY ${td}
  )
set_property(TEST swig_test_time PROPERTY LABELS swig Time CommonTime)

add_test(NAME swig_test_misc
  COMMAND ${PYTHON_EXECUTABLE} test_misc.py
  WORKING_DIRECTORY ${td}
  )
set_property(TEST swig_test_misc PROPERTY LABELS swig ObsID SatID)

#add_test(NAME swig_test_store
#  COMMAND ${PYTHON_EXECUTABLE} test_store.py
#  WORKING_DIRECTORY ${td}
#  )
#set_property(TEST swig_test_store PROPERTY LABELS swig )
