# Test for failure when the output is not restricted for regression,
# e.g. exception text that will vary from release to release.

execute_process(COMMAND ${TEST_PROG} ${SOURCEDIR}/${FILE1} ${SOURCEDIR}/${FILE2}
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}.out
                RESULT_VARIABLE HAD_ERROR)
# files are expected to be different
if(HAD_ERROR EQUAL 0)
    message(FATAL_ERROR "Test failed")
endif()
