# test that help request doesn't fail
# Since bc2sp3 doesn't use the usual app framework, 
# it needs a special check. 

# help alone
execute_process(COMMAND ${TEST_PROG} --help
                OUTPUT_QUIET
                ERROR_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed with long help option")
endif()

# A no argument execution should simply produce the help message
execute_process(COMMAND ${TEST_PROG} 
                OUTPUT_QUIET
                ERROR_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed - returned OK with no arguments")
endif()

# unimplemented option.  Needs two arguments to meet minimum conditions.
execute_process(COMMAND ${TEST_PROG} --whyUimplementDis --whyUimplementDat
                OUTPUT_QUIET
                ERROR_QUIET
                RESULT_VARIABLE HAD_ERROR)
message(STATUS "${TEST_PROG} returned ${HAD_ERROR}")
if(HAD_ERROR EQUAL 0)
    message(FATAL_ERROR "Test failed with undefined option")
endif()
