# test ValidOutput RINEX Met

 message(STATUS "GPSTK_BINDIR: ${GPSTK_BINDIR}, PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
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
message(STATUS "running ${GPSTK_BINDIR}/rmwcheck ${TARGETDIR}/${TESTBASE}.out")

execute_process(COMMAND ${GPSTK_BINDIR}/rmwcheck ${TARGETDIR}/${TESTBASE}.out
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Output was not Rinex Met, exit code: ${HAD_ERROR}")
endif()
