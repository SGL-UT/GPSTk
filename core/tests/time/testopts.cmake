# test that output is correct and unchanged

execute_process(COMMAND ${TEST_PROG} ${PROG_OPTS} ${PROG_OPTS_2}
                OUTPUT_FILE ${TARGETDIR}/${TESTNAME}.out
                RESULT_VARIABLE HAD_ERROR)
# files are expected to be the same
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files
    ${TARGETDIR}/${TESTNAME}.out ${SOURCEDIR}/${TESTBASE}.exp
    RESULT_VARIABLE DIFFERENT)
if(DIFFERENT)
    message(FATAL_ERROR "Test failed - files differ")
endif()
