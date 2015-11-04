# test that missing files cause failure

execute_process(COMMAND ${TEST_PROG} ladfkjlsdkfjladfj lgfjlgjlagjad
                RESULT_VARIABLE HAD_ERROR)
if(!HAD_ERROR)
    message(FATAL_ERROR "Test failed")
endif()
