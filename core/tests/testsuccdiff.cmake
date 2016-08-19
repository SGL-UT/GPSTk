# Generic test where the output of an application is compared using the 
# df_diff tool.
#
# ****NOTE****
# This functionality has been rolled into testsuccexp.
# Please use that instead.
# ****NOTE****
#
#
# Expected variables (required unless otherwise noted):
# TEST_PROG: the program under test
# DIFF_PROG: the program that will difference the output and comparison data
# DIFF_ARGS: arguments to pass to DIFF_PROG
# SOURCEDIR: the location of the reference file
# TARGETDIR: the directory to store stdout to compare with the reference output
# TESTBASE: the name of the test, used to create output file and find reference
# ARGS: a space-separated argument list (optional)


# Convert ARGS into a cmake list
IF(DEFINED ARGS)
   string(REPLACE " " ";" ARG_LIST ${ARGS})
ENDIF(DEFINED ARGS)

message(STATUS "running ${TEST_PROG} ${ARGS}")

execute_process(COMMAND ${TEST_PROG} ${ARG_LIST} 
                 OUTPUT_FILE ${TARGETDIR}/${TESTBASE}.out
                 RESULT_VARIABLE HAD_ERROR)

if(HAD_ERROR)
    message(FATAL_ERROR "Test failed, rc: ${HAD_ERROR}")
endif(HAD_ERROR)


set(out ${TARGETDIR}/${TESTBASE}.out)
set(exp ${SOURCEDIR}/${TESTBASE}.exp)
message(STATUS         "${DIFF_PROG} ${DIFF_ARGS} -1 ${out} -2 ${exp}")
execute_process(COMMAND ${DIFF_PROG} ${DIFF_ARGS} -1 ${out} -2 ${exp}
    RESULT_VARIABLE DIFFERENCE)

if(DIFFERENCE)
    message(FATAL_ERROR "Test failed - Files differ")
else()
    message(STATUS "Test passed")
endif(DIFFERENCE)

