# Notes:
#
# The tests labeled:
# check differences where one file is invalid
# Are not very good.  The exit code of the program is the same if
# there are differences or if one file is of an invalid format.
#
# Currently untested:
#   * Time span specification on r*wdiff command lines

###############################################################################
# TEST rmwdiff
###############################################################################

# check that -h option is valid
add_test(NAME rmwdiff_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwdiff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testhelp.cmake)

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

# check differences where one file is invalid
add_test(NAME rmwdiff_Diff_4
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwdiff>
         -DFILE1=arlm200a.15o
         -DFILE2=arlm200z.15m
         -DTESTBASE=rmwdiff4
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testfail.cmake)

# check differences where the set of observables is different
add_test(NAME rmwdiff_Diff_5
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwdiff>
         -DFILE1=arlm200a.15m
         -DFILE2=arlm200y.15m
         -DTESTBASE=rmwdiff5
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testdiff.cmake)


###############################################################################
# TEST rnwdiff
###############################################################################

# check that -h option is valid
add_test(NAME rnwdiff_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwdiff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testhelp.cmake)

# check that rnwdiff fails with missing files
add_test(NAME rnwdiff_CmdOpt_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwdiff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testmissing.cmake)

# check differences where there is no overlap in time
add_test(NAME rnwdiff_Diff_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwdiff>
         -DFILE1=arlm200a.15n
         -DFILE2=arlm200b.15n
         -DTESTBASE=rnwdiff1
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testdiff.cmake)

# check differences where there are none
add_test(NAME rnwdiff_Diff_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwdiff>
         -DFILE1=arlm2000.15n
         -DFILE2=arlm2001.15n
         -DTESTBASE=rnwdiff2
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsame.cmake)

# check differences where there is time overlap and some data differences
add_test(NAME rnwdiff_Diff_3
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwdiff>
         -DFILE1=arlm200a.15n
         -DFILE2=arlm200z.15n
         -DTESTBASE=rnwdiff3
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testdiff.cmake)

# check differences where one file is invalid
add_test(NAME rnwdiff_Diff_4
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwdiff>
         -DFILE1=arlm200a.15o
         -DFILE2=arlm200z.15n
         -DTESTBASE=rnwdiff4
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testfail.cmake)


###############################################################################
# TEST rowdiff
###############################################################################

# check that -h option is valid
add_test(NAME rowdiff_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowdiff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testhelp.cmake)

# check that rowdiff fails with missing files
add_test(NAME rowdiff_CmdOpt_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowdiff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testmissing.cmake)

# check differences where there is no overlap in time
add_test(NAME rowdiff_Diff_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowdiff>
         -DFILE1=arlm200a.15o
         -DFILE2=arlm200b.15o
         -DTESTBASE=rowdiff1
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testdiff.cmake)

# check differences where there are none
add_test(NAME rowdiff_Diff_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowdiff>
         -DFILE1=arlm200a.15o
         -DFILE2=arlm200x.15o
         -DTESTBASE=rowdiff2
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsame.cmake)

# check differences where there is time overlap and some data differences
add_test(NAME rowdiff_Diff_3
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowdiff>
         -DFILE1=arlm200a.15o
         -DFILE2=arlm200z.15o
         -DTESTBASE=rowdiff3
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testdiff.cmake)

# check differences where one file is invalid
add_test(NAME rowdiff_Diff_4
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowdiff>
         -DFILE1=arlm200a.15n
         -DFILE2=arlm200z.15o
         -DTESTBASE=rowdiff4
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testfail.cmake)


###############################################################################
# TEST rinheaddiff
###############################################################################

# check that -h option is valid
add_test(NAME rinheaddiff_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testhelp.cmake)
