# Generic test where success is expected and standard output is
# compared to a variable.
#
# Expected variables (required unless otherwise noted):
#
# TEST_PROG: the program under test
# EXP_OUT: a string containing the expected output i.e. what will be in TESTOUT
# ARGS: a space-separated argument list (optional)
# SPARG1: a single escaped argument (optional)
# SPARG2: a single escaped argument (optional)
# SPARG3: a single escaped argument (optional)
# SPARG4: a single escaped argument (optional)
# EXP_RC: expected rc from the program (optional)

if(NOT EXP_RC)
    set(EXP_RC 0)
endif()

# Convert ARGS into a cmake list
IF(DEFINED ARGS)
   string(REPLACE " " ";" ARG_LIST ${ARGS})
ENDIF(DEFINED ARGS)


message(STATUS "${TEST_PROG} ${ARGS} ${SPARG1} ${SPARG2} ${SPARG3} ${SPARG4}")
execute_process(COMMAND ${TEST_PROG} ${ARG_LIST} ${SPARG1} ${SPARG2} ${SPARG3} ${SPARG4}
        OUTPUT_VARIABLE TESTOUT
        RESULT_VARIABLE RC)

# remove newline at the end
string(REGEX REPLACE "\n$" "" TESTOUT "${TESTOUT}")

if(NOT TESTOUT STREQUAL EXP_OUT)
    message(foo${TESTOUT}bar)
    message(baz${EXP_OUT}niz)
    message(FATAL_ERROR "Test failed, ${TESTOUT} != ${EXP_OUT}")
endif()

if(NOT EXP_RC EQUAL RC)
    message(FATAL_ERROR "Test failed, ${RC} != ${EXP_RC}")
endif()
