# test that output matches expected format

IF(DEFINED ARGS)
   string(REPLACE " " ";" ARG_LIST ${ARGS})
ENDIF(DEFINED ARGS)

message(STATUS "running ${TEST_PROG} ${ARGS}")
execute_process(COMMAND ${TEST_PROG} ${ARG_LIST} 
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Program did not produce output.")
endif()

# print the command for debugging purposes
message(STATUS "running ${CHECK_TOOL} ${TARGETDIR}/${TESTBASE}.out")

execute_process(COMMAND ${CHECK_TOOL} ${TARGETDIR}/${TESTBASE}.out
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Output was not of the expected format, exit code: ${HAD_ERROR}")
endif()
