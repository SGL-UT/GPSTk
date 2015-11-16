###############################################################################
# TEST mpsolve
###############################################################################

# check that -h option is valid
add_test(NAME mpsolve_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:mpsolve>
         -DSOURCEDIR=${SGLTK_TEST_DATA_DIR}
         -DTARGETDIR=${SGLTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testhelp.cmake)
