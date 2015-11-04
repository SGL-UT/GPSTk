# test that files are the same

execute_process(COMMAND ${TEST_PROG} ${SOURCEDIR}/${FILE1} ${SOURCEDIR}/${FILE2}
                OUTPUT_FILE ${TARGETDIR}/${TESTBASE}.out
                RESULT_VARIABLE HAD_ERROR)
# files are expected to be the same
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files
    ${TARGETDIR}/${TESTBASE}.out ${SOURCEDIR}/${TESTBASE}.exp
    RESULT_VARIABLE DIFFERENT)
if(DIFFERENT)
    message(FATAL_ERROR "Test failed - files differ")
endif()
