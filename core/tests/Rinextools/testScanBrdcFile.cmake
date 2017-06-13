# scanBrdcFile is a bit different from many other programs so this is a specific
# test for it. It doesn't attempt to do any real parameter passing since I'm
# not sure how much value there is in that...

# Required variables
# TEST_PROG: the program under test
# SOURCEDIR: the location of the reference file
# TARGETDIR: the directory to store stdout to compare with the reference output
# TESTBASE: the name of the test, used to create output file and find reference
# DIFF_PROG: path to the df_diff program

set(in1 ${SOURCEDIR}/inputs/brdc0070.16n)
set(in2 ${SOURCEDIR}/inputs/brdc0080.16n)

set(out1 ${TARGETDIR}/${TESTBASE}-007.out)
set(out2 ${TARGETDIR}/${TESTBASE}-008.out)
set(out3 ${TARGETDIR}/${TESTBASE}-sum.out)

set(exp1 ${SOURCEDIR}/outputs/${TESTBASE}-007.exp)
set(exp2 ${SOURCEDIR}/outputs/${TESTBASE}-008.exp)
set(exp3 ${SOURCEDIR}/outputs/${TESTBASE}-sum.exp)

message(STATUS         "${TEST_PROG} -i ${in1} -i ${in2} -o ${out1} -o ${out2} -s ${out3}")
execute_process(COMMAND ${TEST_PROG} -i ${in1} -i ${in2} -o ${out1} -o ${out2} -s ${out3}
    RESULT_VARIABLE RC)

if(NOT RC EQUAL 0)
    message(FATAL_ERROR "Test failed, ${RC} != 0")
endif()

message(STATUS         "${DIFF_PROG} -v -l2 -1${out1} -2${exp1}")
execute_process(COMMAND ${DIFF_PROG} -v -l2 -1${out1} -2${exp1} RESULT_VARIABLE RC)

if(RC)
    message(FATAL_ERROR "Test failed, file differs")
endif()

message(STATUS         "${DIFF_PROG} -v -l2 -1${out2} -2${exp2}")
execute_process(COMMAND ${DIFF_PROG} -v -l2 -1${out2} -2${exp2} RESULT_VARIABLE RC)

if(RC)
    message(FATAL_ERROR "Test failed, file differs")
endif()

message(STATUS         "${DIFF_PROG} -v -l4 -1${out3} -2${exp3}")
execute_process(COMMAND ${DIFF_PROG} -v -l4 -1${out3} -2${exp3} RESULT_VARIABLE RC)

if(RC)
    message(FATAL_ERROR "Test failed, file differs")
endif()
