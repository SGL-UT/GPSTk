# Generic test where success is expected and standard output is
# compared to a reference file.
#
# Expected variables (required unless otherwise noted):
# SOURCEDIR: the location of the reference file
# TARGETDIR: the directory to store stdout to compare with the reference output
# TESTNAME: the name of the test, used to create the output files
#   (must be unique, otherwise parallel tests can fail at random)
# TESTBASE: the name of the test providing the reference data
#
# TEST_PROG: the program under test
# ARGS: a space-separated argument list (optional)
# SPARG1: a single escaped argument (optional)
# SPARG2: a single escaped argument (optional)
# SPARG3: a single escaped argument (optional)
# SPARG4: a single escaped argument (optional)
# EXP_RC: expected rc from the program (optional)

# DIFF_PROG: if defined, use this for differenceing the outputs
# DIFF_ARGS: arguments to pass to ${DIFF_PROG}
#
# Flags:
# OWNOUTPUT: if unset, stdout will be captured to ${TESTNAME}.out.  If
#    set, it is expected that the application itself will produce
#    ${TESTNAME}.out.
# NODIFF: if set, do not compare the output to a reference file as
#    part of the test.
# DIFFSTDERR: if set, the stderr output will be compared to a reference file

if(NOT EXP_RC)
    set(EXP_RC 0)
endif()

# Convert ARGS into a cmake list
IF(DEFINED ARGS)
   string(REPLACE " " ";" ARG_LIST ${ARGS})
ENDIF(DEFINED ARGS)


IF(NOT DEFINED OWNOUTPUT)
    IF(NOT DEFINED TESTNAME)
       message(FATAL_ERROR "Test failed, TESTNAME is not set (stdout)")
    ENDIF(NOT DEFINED TESTNAME)
    message(STATUS "${TEST_PROG} ${ARGS} ${SPARG1} ${SPARG2} ${SPARG3} ${SPARG4} >${TARGETDIR}/${TESTNAME}.out")
    execute_process(COMMAND ${TEST_PROG} ${ARG_LIST} ${SPARG1} ${SPARG2} ${SPARG3} ${SPARG4}
        OUTPUT_FILE ${TARGETDIR}/${TESTNAME}.out
        ERROR_FILE ${TARGETDIR}/${TESTNAME}.err
        RESULT_VARIABLE RC)
ELSE(NOT DEFINED OWNOUTPUT)
    message(STATUS "${TEST_PROG} ${ARGS} ${SPARG1} ${SPARG2} ${SPARG3} ${SPARG4}")
    execute_process(COMMAND ${TEST_PROG} ${ARG_LIST} ${SPARG1} ${SPARG2} ${SPARG3} ${SPARG4}
        OUTPUT_QUIET
        RESULT_VARIABLE RC)
ENDIF(NOT DEFINED OWNOUTPUT)

if(NOT EXP_RC EQUAL RC)
    message(FATAL_ERROR "Test failed, ${EXP_RC} != ${RC}")
endif()


IF(NOT DEFINED NODIFF)
    IF(NOT DEFINED TESTBASE)
       message(FATAL_ERROR "Test failed, TESTBASE is not set (regular)")
    ENDIF(NOT DEFINED TESTBASE)
    IF(NOT DEFINED TESTNAME)
       message(FATAL_ERROR "Test failed, TESTNAME is not set (regular)")
    ENDIF(NOT DEFINED TESTNAME)
    set(exp "${SOURCEDIR}/${TESTBASE}.exp")
    set(out "${TARGETDIR}/${TESTNAME}.out")

    if(DEFINED DIFF_PROG)
        message(STATUS         "${DIFF_PROG} ${DIFF_ARGS} -1 ${out} -2 ${exp}")
        execute_process(COMMAND ${DIFF_PROG} ${DIFF_ARGS} -1 ${out} -2 ${exp}
            RESULT_VARIABLE DIFFERENT)
    else()
        message(STATUS "diff ${out} ${exp}")
        execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${out} ${exp}
            RESULT_VARIABLE DIFFERENT)
    endif()
    
   if(DIFFERENT)
       message(FATAL_ERROR "Test failed - files differ: ${DIFFERENT}")
   else()
       message(STATUS "Test passed")
   endif(DIFFERENT)
ENDIF(NOT DEFINED NODIFF)


IF(DEFINED DIFFSTDERR)
    IF(NOT DEFINED TESTBASE)
       message(FATAL_ERROR "Test failed, TESTBASE is not set (stderr)")
    ENDIF(NOT DEFINED TESTBASE)
    IF(NOT DEFINED TESTNAME)
       message(FATAL_ERROR "Test failed, TESTNAME is not set (stderr)")
    ENDIF(NOT DEFINED TESTNAME)
    set(exp "${SOURCEDIR}/${TESTBASE}.err.exp")
    set(out "${TARGETDIR}/${TESTNAME}.err")

    if(DEFINED DIFF_PROG)
        message(STATUS         "${DIFF_PROG} ${DIFF_ARGS} -1 ${out} -2 ${exp}")
        execute_process(COMMAND ${DIFF_PROG} ${DIFF_ARGS} -1 ${out} -2 ${exp}
            RESULT_VARIABLE DIFFERENT)
    else()
        message(STATUS "diff ${out} ${exp}")
        execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${out} ${exp}
            RESULT_VARIABLE DIFFERENT)
    endif()
    
   if(DIFFERENT)
       message(FATAL_ERROR "Test failed - files differ: ${DIFFERENT}")
   else()
       message(STATUS "Test passed")
   endif(DIFFERENT)
ENDIF(DEFINED DIFFSTDERR)
