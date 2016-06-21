###############################################################################
# Test for mpsolve Multipath Environment Evaluator
#
# Expected variables (required unless otherwise noted):
# TEST_PROG: the program under test
# SOURCEDIR: the location of the reference file
# TARGETDIR: the directory to store stdout to compare with the reference output
# TESTBASE: the name of the test, used to create output file and find reference
# IN_OBS_FILE1: first input file
# IN_NAV_FILE1: second input file
#
# TEST_PROG is expected to generate the output file
# ${TARGETDIR}/${TESTBASE}.out
#
# Reference file is ${SOURCEDIR}/${TESTBASE}.exp
###############################################################################

message(STATUS "Running ${TEST_PROG} -o ${TARGETDIR}/${TESTBASE}.out ${SOURCEDIR}/${IN_OBS_FILE1} ${SOURCEDIR}/${IN_NAV_FILE1}")


###############################################################################
# Ensure required options -o and -e are present
###############################################################################
execute_process(COMMAND ${TEST_PROG} -o ${IN_OBS_FILE1}
                OUTPUT_QUIET
                ERROR_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(!HAD_ERROR)
    message(FATAL_ERROR "Test failed due to missing required option -e")
endif()

execute_process(COMMAND ${TEST_PROG} -e ${IN_NAV_FILE1}
                OUTPUT_QUIET
                ERROR_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(!HAD_ERROR)
    message(FATAL_ERROR "Test failed due to missing required option -o")
endif()

execute_process(COMMAND ${TEST_PROG} -o ${IN_OBS_FILE1} -e ${IN_NAV_FILE1}
                OUTPUT_QUIET
                ERROR_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed")
endif()


###############################################################################
###############################################################################

# @todo - Check that -n/--numeric is handled properly

# @todo - Check that -r/--raw is handled properly

# @todo - Check that -a/--azimuth is handled properly

# @todo - Check that -c/--complete is handled properly

# @todo - Check that -d/--dfm is handled properly

# @todo - Check that -o/--obs is handled properly

# @todo - Check that -e/--nav is handled properly

# @todo - Check that -b/--bin is handled properly

# @todo - Check that -m/--multipath is handled properly

# @todo - Check that -u/--upper is handled properly

# @todo - Check that -p/--plot is handled properly

# @todo - Check that -f/--file is handled properly

# @todo - Check that -l/--length is handled properly

# @todo - Check that -w/--width is handled properly

# @todo - Check that --view is handled properly
