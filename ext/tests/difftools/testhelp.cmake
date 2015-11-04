# test that help request doesn't fail

execute_process(COMMAND ${TEST_PROG} -h
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed")
endif()
