###############################################################################
# TEST calgps
###############################################################################

# These tests check both that options haven't changed and output
# format hasn't changed.

# Check that -h option is valid.  calgps doesn't use BasicFramework so
# we don't do the usual checks of verbose and debug and so on.
add_test(NAME calgps_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_CmdOpt_1
         -DARGS=-h
         -DNODIFF=1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testsuccexp.cmake)

# Check that -3 option is valid.
add_test(NAME calgps_CmdOpt_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_CmdOpt_2
         -DARGS=-3
         -DNODIFF=1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testsuccexp.cmake)

# Check that -y option is valid.
add_test(NAME calgps_CmdOpt_3
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_CmdOpt_3
         -DARGS=-y
         -DNODIFF=1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testsuccexp.cmake)

# Check that no options is valid.
add_test(NAME calgps_CmdOpt_4
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_CmdOpt_4
         -DNODIFF=1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testsuccexp.cmake)

# Check that -Y produces expected output.
add_test(NAME calgps_Year
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_Year
         -DARGS=-Y\ 2015
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testsuccexp.cmake)

# Check that -p produces expected output.
add_test(NAME calgps_Postscript
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_Postscript
         -DOWNOUTPUT=1
         -DARGS=-Y\ 2015\ -p\ ${GPSTK_TEST_OUTPUT_DIR}/calgps_Postscript.out
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testsuccexp.cmake)

# Check that -n with -p produces expected output.
add_test(NAME calgps_Postscript_Quiet
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_Postscript_Quiet
         -DOWNOUTPUT=1
         -DARGS=-n\ -Y\ 2015\ -p\ ${GPSTK_TEST_OUTPUT_DIR}/calgps_Postscript_Quiet.out
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testsuccexp.cmake)

# Check that -e produces expected output.
add_test(NAME calgps_EPS
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_EPS
         -DOWNOUTPUT=1
         -DARGS=-Y\ 2015\ -e\ ${GPSTK_TEST_OUTPUT_DIR}/calgps_EPS.out
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testsuccexp.cmake)

# Check that -s produces expected output.
add_test(NAME calgps_SVG
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_SVG
         -DOWNOUTPUT=1
         -DARGS=-Y\ 2015\ -s\ ${GPSTK_TEST_OUTPUT_DIR}/calgps_SVG.out
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testsuccexp.cmake)
