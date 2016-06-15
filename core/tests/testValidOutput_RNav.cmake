# test ValidOutput RINEX Nav

# message(STATUS "${GPSTK_BINDIR}")
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
message(STATUS "running ${GPSTK_BINDIR}/rnwcheck ${TARGETDIR}/${TESTBASE}.out")

execute_process(COMMAND ${GPSTK_BINDIR}/rnwcheck ${TARGETDIR}/${TESTBASE}.out
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Output was not Rinex Nav, exit code: ${HAD_ERROR}")
endif()
