###############################################################################
# TEST geomatics applications: DDBase and DiscFix
###############################################################################

# DDBase: Check that -h, -v, and -d options are handled properly
add_test(NAME DDBase_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DDBase>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_SOURCE_DIR}/core/tests/testhelp.cmake)

# DiscFix: Check that -h, -v, and -d options are handled properly
add_test(NAME DiscFix_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DiscFix>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_SOURCE_DIR}/core/tests/testhelp.cmake)
