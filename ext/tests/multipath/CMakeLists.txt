###############################################################################
# TEST mpsolve_svg
###############################################################################

# Check that -h, -v, and -d options are handled properly
add_test(NAME mpsolve_svg_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:mpsolve_svg>
         -DSOURCEDIR=${SGLTK_TEST_DATA_DIR}
         -DTARGETDIR=${SGLTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testhelp.cmake)
