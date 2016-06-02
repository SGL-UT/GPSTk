# Test for RINEX merge tools.
#
# Expected variables (required unless otherwise noted):
# TEST_PROG: the program under test
# SOURCEDIR: the location of the reference file
# TARGETDIR: the directory to store stdout to compare with the reference output
# TESTBASE: the name of the test, used to create output file and find reference
# INFILE1: first input file
# INFILE2: second input file
# RINDIFF: location of RINEX diff tool for the format being tested
# RINHEADDIFF: location of rinheaddiff application
#
# TEST_PROG is expected to generate the output file
# ${TARGETDIR}/${TESTBASE}.out
#
# Reference file is ${SOURCEDIR}/${TESTBASE}.exp

# Generate the merged file

message(STATUS "running ${TEST_PROG} -o ${TARGETDIR}/${TESTBASE}.out ${SOURCEDIR}/${INFILE1} ${SOURCEDIR}/${INFILE2}")

execute_process(COMMAND ${TEST_PROG} -o ${TARGETDIR}/${TESTBASE}.out ${SOURCEDIR}/${INFILE1} ${SOURCEDIR}/${INFILE2}
                OUTPUT_QUIET
                RESULT_VARIABLE HAD_ERROR)
if(HAD_ERROR)
    message(FATAL_ERROR "Test failed, exit code: ${HAD_ERROR}")
endif()


# diff against reference

message(STATUS "running ${RINDIFF} ${SOURCEDIR}/${TESTBASE}.exp ${TARGETDIR}/${TESTBASE}.out")

execute_process(COMMAND ${RINDIFF} ${SOURCEDIR}/${TESTBASE}.exp ${TARGETDIR}/${TESTBASE}.out
    OUTPUT_QUIET
    RESULT_VARIABLE DIFFERENT)
if(DIFFERENT)
    message(FATAL_ERROR "Test failed - files differ")
endif()


# Check for header differences

set( EXCL1 "PGM / RUN BY / DATE" )

message(STATUS "running ${RINHEADDIFF} -x ${EXCL1} ${SOURCEDIR}/${TESTBASE}.exp ${TARGETDIR}/${TESTBASE}.out")

execute_process(COMMAND ${RINHEADDIFF} -x ${EXCL1} ${SOURCEDIR}/${TESTBASE}.exp ${TARGETDIR}/${TESTBASE}.out
    RESULT_VARIABLE DIFFERENT)
if(DIFFERENT)
    message(FATAL_ERROR "Test failed - headers differ")
endif()
