# Generic test where the output of an application is compared using the 
# df_diff tool.
#
# Expected variables (required unless otherwise noted):
# TEST_PROG: the program under test
# DIFF_PROG: the program that will difference the output and comparison data
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
                 RESULT_VARIABLE HAD_ERROR)
 if(HAD_ERROR)
     message(FATAL_ERROR "Test failed, exit code: ${HAD_ERROR}")
 endif(HAD_ERROR)

message(STATUS "running ${DIFF_PROG}")

execute_process(COMMAND ${DIFF_PROG}
    -1 ${SOURCEDIR}/${TESTBASE}.exp
    -2 ${TARGETDIR}/rinexout.exp
    -v
    RESULT_VARIABLE DIFFERENCE)
if(DIFFERENCE)
    message(FATAL_ERROR "Test failed - Files differ")
else()
    message(STATUS "Test passed")
endif(DIFFERENCE)

