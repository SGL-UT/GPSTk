###############################################################################
# TEST geomatics applications: DDBase and DiscFix
###############################################################################

# DDBase: Check that -h/--help is handled properly
add_test(NAME DDBase_CmdOpt_help
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DDBase>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_SOURCE_DIR}/core/tests/testhelp.cmake)

# @todo - DDBase: Check that all other command line options are handled properly

# DiscFix: Check that -h/--help is handled properly
add_test(NAME DiscFix_CmdOpt_help
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:DiscFix>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_SOURCE_DIR}/core/tests/testhelp.cmake)

# @todo - DiscFix: Check that all other command line options are handled properly
