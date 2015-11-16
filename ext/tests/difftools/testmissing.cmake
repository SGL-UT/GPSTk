# test that missing files cause failure

message(STATUS "running ${TEST_PROG} ladfkjlsdkfjladfj lgfjlgjlagjad")

execute_process(COMMAND ${TEST_PROG} ladfkjlsdkfjladfj lgfjlgjlagjad
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
    message(FATAL_ERROR "Test failed")
endif()
