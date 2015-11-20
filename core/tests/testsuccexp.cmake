# Generic test where success is expected and standard output is
# compared to a reference file.
#
# Expected variables (required unless otherwise noted):
# TEST_PROG: the program under test
# SOURCEDIR: the location of the reference file
# TARGETDIR: the directory to store stdout to compare with the reference output
# TESTBASE: the name of the test, used to create output file and find reference
# ARGS: a space-separated argument list (optional)
# SPARG1: a single escaped argument (optional)
# SPARG2: a single escaped argument (optional)
# SPARG3: a single escaped argument (optional)
# SPARG4: a single escaped argument (optional)
#
# Flags:
# OWNOUTPUT: if unset, stdout will be captured to ${TESTBASE}.out.  If
#    set, it is expected that the application itself will produce
#    ${TESTBASE}.out.
# NODIFF: if set, do not compare the output to a reference file as
#    part of the test.


# Convert ARGS into a cmake list
IF(DEFINED ARGS)
   string(REPLACE " " ";" ARG_LIST ${ARGS})
ENDIF(DEFINED ARGS)

message(STATUS "running ${TEST_PROG} ${ARGS} ${SPARG1} ${SPARG2} ${SPARG3} ${SPARG4}")

IF(NOT DEFINED OWNOUTPUT)
   execute_process(COMMAND ${TEST_PROG} ${ARG_LIST} ${SPARG1} ${SPARG2} ${SPARG3} ${SPARG4}
                   OUTPUT_FILE ${TARGETDIR}/${TESTBASE}.out
                   RESULT_VARIABLE HAD_ERROR)
   if(HAD_ERROR)
       message(FATAL_ERROR "Test failed, exit code: ${HAD_ERROR}")
   endif(HAD_ERROR)
ELSE(NOT DEFINED OWNOUTPUT)
   execute_process(COMMAND ${TEST_PROG} ${ARG_LIST} ${SPARG1} ${SPARG2} ${SPARG3} ${SPARG4}
                   OUTPUT_QUIET
                   RESULT_VARIABLE HAD_ERROR)
   if(HAD_ERROR)
       message(FATAL_ERROR "Test failed, exit code: ${HAD_ERROR}")
   endif(HAD_ERROR)
ENDIF(NOT DEFINED OWNOUTPUT)

IF(NOT DEFINED NODIFF)
   execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files
       ${SOURCEDIR}/${TESTBASE}.exp ${TARGETDIR}/${TESTBASE}.out
       RESULT_VARIABLE DIFFERENT)
   if(DIFFERENT)
       message(FATAL_ERROR "Test failed - files differ: ${DIFFERENT}")
   endif(DIFFERENT)
ENDIF(NOT DEFINED NODIFF)
