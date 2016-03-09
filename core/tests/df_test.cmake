# Tests for the df_test program


execute_process(COMMAND
    ${TEST_PROG}
    -1 ${GPSTK_TEST_DATA_DIR}/arlm200b.15o
    -2 ${GPSTK_TEST_DATA_DIR}/df_diff.in
    -vvv
    RESULT_VARIABLE HAD_ERROR)

if(HAD_ERROR)
    message(FATAL_ERROR "Test failed\n${HAD_ERROR}")
else()
    message(STATUS "Test passed")
endif()
