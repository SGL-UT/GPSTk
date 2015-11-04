###############################################################################
# TEST rmwdiff
###############################################################################

# check that -h option is valid
add_test(NAME rmwdiff_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwdiff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testhelp.cmake)

# check that rmwdiff fails with missing files
add_test(NAME rmwdiff_CmdOpt_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwdiff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testmissing.cmake)

# check differences where there is no overlap in time
add_test(NAME rmwdiff_Diff_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwdiff>
         -DFILE1=arlm200a.15m
         -DFILE2=arlm200b.15m
         -DTESTBASE=rmwdiff1
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testdiff.cmake)

# check differences where there are none
add_test(NAME rmwdiff_Diff_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwdiff>
         -DFILE1=arlm2000.15m
         -DFILE2=arlm2001.15m
         -DTESTBASE=rmwdiff2
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsame.cmake)

# check differences where there is time overlap and some data differences
add_test(NAME rmwdiff_Diff_3
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwdiff>
         -DFILE1=arlm200a.15m
         -DFILE2=arlm200z.15m
         -DTESTBASE=rmwdiff3
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testdiff.cmake)
