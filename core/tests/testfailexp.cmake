# Generic test where failure is expected
# stick a space-separated argument list in ARGS


# Convert ARGS into a cmake list
IF(DEFINED ARGS)
   string(REPLACE " " ";" ARG_LIST ${ARGS})
ENDIF(DEFINED ARGS)

execute_process(COMMAND ${TEST_PROG} ${ARG_LIST}
                OUTPUT_QUIET
                ERROR_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR EQUAL 0)
    message(FATAL_ERROR "Test failed")
endif()
