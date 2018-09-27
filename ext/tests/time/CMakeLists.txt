###############################################################################
# TEST calgps_svg
###############################################################################

# Check that -p produces expected output.
add_test(NAME calgps_Postscript
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps_svg>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_Postscript
         -DOWNOUTPUT=1
         -DARGS=-Y\ 2015\ -p\ ${GPSTK_TEST_OUTPUT_DIR}/calgps_Postscript.out
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testsuccexp.cmake)

# Check that -n with -p produces expected output.
add_test(NAME calgps_Postscript_Quiet
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps_svg>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_Postscript_Quiet
         -DOWNOUTPUT=1
         -DARGS=-n\ -Y\ 2015\ -p\ ${GPSTK_TEST_OUTPUT_DIR}/calgps_Postscript_Quiet.out
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testsuccexp.cmake)

# Check that -e produces expected output.
add_test(NAME calgps_EPS
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps_svg>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_EPS
         -DOWNOUTPUT=1
         -DARGS=-Y\ 2015\ -e\ ${GPSTK_TEST_OUTPUT_DIR}/calgps_EPS.out
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testsuccexp.cmake)

# Check that -s produces expected output.
add_test(NAME calgps_SVG
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:calgps_svg>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=calgps_SVG
         -DOWNOUTPUT=1
         -DARGS=-Y\ 2015\ -s\ ${GPSTK_TEST_OUTPUT_DIR}/calgps_SVG.out
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testsuccexp.cmake)
