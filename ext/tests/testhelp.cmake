# test that help request doesn't fail
# also checks acceptance of debug and verbose options
# and an option that shouldn't be implemented anywhere

# help alone

execute_process(COMMAND ${TEST_PROG} -h
                OUTPUT_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed with short help option")
endif()

execute_process(COMMAND ${TEST_PROG} --help
                OUTPUT_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed with long help option")
endif()

# help and debug

execute_process(COMMAND ${TEST_PROG} -h -d
                OUTPUT_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed with short debug option")
endif()

execute_process(COMMAND ${TEST_PROG} -h -ddd
                OUTPUT_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed with multiple short debug options")
endif()

# help and long debug

execute_process(COMMAND ${TEST_PROG} -h --debug
                OUTPUT_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed with long debug option")
endif()

execute_process(COMMAND ${TEST_PROG} -h --debug --debug --debug
                OUTPUT_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed with multiple long debug options")
endif()

# help and verbose

execute_process(COMMAND ${TEST_PROG} -h -v
                OUTPUT_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed with short verbose option")
endif()

execute_process(COMMAND ${TEST_PROG} -h -vvv
                OUTPUT_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed with multiple short verbose options")
endif()

# help and long verbose

execute_process(COMMAND ${TEST_PROG} -h --verbose
                OUTPUT_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed with long verbose option")
endif()

execute_process(COMMAND ${TEST_PROG} -h --verbose --verbose --verbose
                OUTPUT_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed with multiple long verbose options")
endif()

# unimplemented option

execute_process(COMMAND ${TEST_PROG} -h --whyUimplementDis
                OUTPUT_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(!HAD_ERROR)
    message(FATAL_ERROR "Test failed with undefined option")
endif()
