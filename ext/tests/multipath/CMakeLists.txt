###############################################################################
# TEST mpsolve_svg
###############################################################################

# Check that -h, -v, and -d options are handled properly
add_test(NAME mpsolve_CmdOpt_help
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:mpsolve>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testhelp.cmake)


# Check that all other command options are handled properly
add_test(NAME mpsolve_CmdOpt_all
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:mpsolve>
         -DTESTBASE=mpsolve_CmdOpt_2
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DIN_OBS_FILE1=test_input_mpsolve.15o
         -DIN_NAV_FILE1=test_input_mpsolve.15n
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testmpsolve.cmake)
