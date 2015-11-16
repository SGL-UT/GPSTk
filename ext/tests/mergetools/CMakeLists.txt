###############################################################################
# TEST mergeRinMet
###############################################################################

# check that -h option is valid
add_test(NAME mergeRinMet_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:mergeRinMet>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testhelp.cmake)

add_test(NAME mergeRinMet_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:mergeRinMet>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=mergeRinMet_1
         -DRINHEADDIFF=$<TARGET_FILE:rinheaddiff>
         -DRINDIFF=$<TARGET_FILE:rmwdiff>
         -DINFILE1=arlm200a.15m
         -DINFILE2=arlm200b.15m
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinmerge.cmake)


###############################################################################
# TEST mergeRinNav
###############################################################################

# check that -h option is valid
add_test(NAME mergeRinNav_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:mergeRinNav>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testhelp.cmake)

add_test(NAME mergeRinNav_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:mergeRinNav>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=mergeRinNav_1
         -DRINHEADDIFF=$<TARGET_FILE:rinheaddiff>
         -DRINDIFF=$<TARGET_FILE:rnwdiff>
         -DINFILE1=arlm200a.15n
         -DINFILE2=arlm200b.15n
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinmerge.cmake)


###############################################################################
# TEST mergeRinObs
###############################################################################

# check that -h option is valid
add_test(NAME mergeRinObs_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:mergeRinObs>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testhelp.cmake)

add_test(NAME mergeRinObs_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:mergeRinObs>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=mergeRinObs_1
         -DRINHEADDIFF=$<TARGET_FILE:rinheaddiff>
         -DRINDIFF=$<TARGET_FILE:rowdiff>
         -DINFILE1=arlm200a.15o
         -DINFILE2=arlm200b.15o
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinmerge.cmake)
