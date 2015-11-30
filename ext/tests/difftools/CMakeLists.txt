set( SD ${GPSTK_TEST_DATA_DIR} )
set( TD ${GPSTK_TEST_OUTPUT_DIR} )

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
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testhelp.cmake)

# check that rmwdiff fails with missing files
add_test(NAME rmwdiff_CmdOpt_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwdiff>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testmissing.cmake)

# check differences where there is no overlap in time
add_test(NAME rmwdiff_Diff_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwdiff>
         -DFILE1=arlm200a.15m
         -DFILE2=arlm200b.15m
         -DTESTBASE=rmwdiff1
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testdiff.cmake)

# check differences where there are none
add_test(NAME rmwdiff_Diff_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwdiff>
         -DFILE1=arlm2000.15m
         -DFILE2=arlm2001.15m
         -DTESTBASE=rmwdiff2
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsame.cmake)

# check differences where there is time overlap and some data differences
add_test(NAME rmwdiff_Diff_3
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwdiff>
         -DFILE1=arlm200a.15m
         -DFILE2=arlm200z.15m
         -DTESTBASE=rmwdiff3
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testdiff.cmake)

# check differences where one file is invalid
add_test(NAME rmwdiff_Diff_4
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwdiff>
         -DFILE1=arlm200a.15o
         -DFILE2=arlm200z.15m
         -DTESTBASE=rmwdiff4
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testfail.cmake)

# check differences where the set of observables is different
add_test(NAME rmwdiff_Diff_5
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rmwdiff>
         -DFILE1=arlm200a.15m
         -DFILE2=arlm200y.15m
         -DTESTBASE=rmwdiff5
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsame.cmake)


###############################################################################
# TEST rnwdiff
###############################################################################

# check that -h option is valid
add_test(NAME rnwdiff_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwdiff>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testhelp.cmake)

# check that rnwdiff fails with missing files
add_test(NAME rnwdiff_CmdOpt_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwdiff>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testmissing.cmake)

# check differences where there is no overlap in time
add_test(NAME rnwdiff_Diff_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwdiff>
         -DFILE1=arlm200a.15n
         -DFILE2=arlm200b.15n
         -DTESTBASE=rnwdiff1
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testdiff.cmake)

# check differences where there are none
add_test(NAME rnwdiff_Diff_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwdiff>
         -DFILE1=arlm2000.15n
         -DFILE2=arlm2001.15n
         -DTESTBASE=rnwdiff2
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsame.cmake)

# check differences where there is time overlap and some data differences
add_test(NAME rnwdiff_Diff_3
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwdiff>
         -DFILE1=arlm200a.15n
         -DFILE2=arlm200z.15n
         -DTESTBASE=rnwdiff3
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testdiff.cmake)

# check differences where one file is invalid
add_test(NAME rnwdiff_Diff_4
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rnwdiff>
         -DFILE1=arlm200a.15o
         -DFILE2=arlm200z.15n
         -DTESTBASE=rnwdiff4
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testfail.cmake)


###############################################################################
# TEST rowdiff
###############################################################################

# check that -h option is valid
add_test(NAME rowdiff_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowdiff>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testhelp.cmake)

# check that rowdiff fails with missing files
add_test(NAME rowdiff_CmdOpt_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowdiff>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testmissing.cmake)

# check differences where there is no overlap in time
add_test(NAME rowdiff_Diff_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowdiff>
         -DFILE1=arlm200a.15o
         -DFILE2=arlm200b.15o
         -DTESTBASE=rowdiff1
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testdiff.cmake)

# check differences where there are none
add_test(NAME rowdiff_Diff_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowdiff>
         -DFILE1=arlm200a.15o
         -DFILE2=arlm200x.15o
         -DTESTBASE=rowdiff2
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsame.cmake)

# check differences where there is time overlap and some data differences
add_test(NAME rowdiff_Diff_3
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowdiff>
         -DFILE1=arlm200a.15o
         -DFILE2=arlm200z.15o
         -DTESTBASE=rowdiff3
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testdiff.cmake)

# check differences where one file is invalid
add_test(NAME rowdiff_Diff_4
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowdiff>
         -DFILE1=arlm200a.15n
         -DFILE2=arlm200z.15o
         -DTESTBASE=rowdiff4
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testfail.cmake)


###############################################################################
# TEST rinheaddiff (RINEX 3 OBS)
###############################################################################

# check that -h option is valid
add_test(NAME rinheaddiff_CmdOpt_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DSOURCEDIR=${SD}
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/tests/testhelp.cmake)

# Make sure that the same file is the same file...
add_test(NAME rinheaddiff_3OBS_Diff_0
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DFILE1=RINEX3_OBS_ref.txt
         -DFILE2=RINEX3_OBS_ref.txt
         -DTESTBASE=rinheaddiff_3obs_0
         -DSOURCEDIR=${SD}/rinheaddiff
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinheadsame.cmake)

# Make sure that headers with irrelevant changes (comment order,
# format name) are considered the same...
add_test(NAME rinheaddiff_3OBS_Diff_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DFILE1=RINEX3_OBS_ref.txt
         -DFILE2=RINEX3_OBS_1.txt
         -DTESTBASE=rinheaddiff_3obs_1
         -DSOURCEDIR=${SD}/rinheaddiff
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinheadsame.cmake)

# Make sure differences are recognized
add_test(NAME rinheaddiff_3OBS_Diff_2a
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DFILE1=RINEX3_OBS_ref.txt
         -DFILE2=RINEX3_OBS_2.txt
         -DTESTBASE=rinheaddiff_3obs_2a
         -DSOURCEDIR=${SD}/rinheaddiff
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinheaddiff.cmake)

# Make sure differences are recognized with one exclusion
set( OPT_2B --exclude=COMMENT )
add_test(NAME rinheaddiff_3OBS_Diff_2b
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DFILE1=RINEX3_OBS_ref.txt
         -DFILE2=RINEX3_OBS_2.txt
         -DIXOPTS=${OPT_2B}
         -DTESTBASE=rinheaddiff_3obs_2b
         -DSOURCEDIR=${SD}/rinheaddiff
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinheaddiff.cmake)

# Make sure differences are recognized with one inclusion
set( OPT_2C --include=COMMENT )
add_test(NAME rinheaddiff_3OBS_Diff_2c
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DFILE1=RINEX3_OBS_ref.txt
         -DFILE2=RINEX3_OBS_2.txt
         -DIXOPTS=${OPT_2C}
         -DTESTBASE=rinheaddiff_3obs_2c
         -DSOURCEDIR=${SD}/rinheaddiff
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinheaddiff.cmake)

# Make sure sameness is recognized with one inclusion
set( OPT_2D "LEAP SECONDS" )
add_test(NAME rinheaddiff_3OBS_Diff_2d
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DFILE1=RINEX3_OBS_ref.txt
         -DFILE2=RINEX3_OBS_2.txt
         -DIXOPTS=--include=${OPT_2D}
         -DTESTBASE=rinheaddiff_3obs_2d
         -DSOURCEDIR=${SD}/rinheaddiff
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinheadsame.cmake)

# Make sure sameness is recognized when excluding all differences.
# Uses a separate script because I can't be bothered to spend any more
# time trying to figure out how to get escaped parameters to the app.
add_test(NAME rinheaddiff_3OBS_Diff_2e
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DFILE1=RINEX3_OBS_ref.txt
         -DFILE2=RINEX3_OBS_2.txt
         -DIXOPTS=--include=${OPT_2E}
         -DTESTBASE=rinheaddiff_3obs_2e
         -DSOURCEDIR=${SD}/rinheaddiff
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinhead2e.cmake)


###############################################################################
# TEST rinheaddiff (RINEX 2 OBS)
###############################################################################

# Make sure that the same file is the same file...
add_test(NAME rinheaddiff_2OBS_Diff_0
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DFILE1=RINEX2_OBS_ref.txt
         -DFILE2=RINEX2_OBS_ref.txt
         -DTESTBASE=rinheaddiff_2obs_0
         -DSOURCEDIR=${SD}/rinheaddiff
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinheadsame.cmake)

# Make sure that headers with irrelevant changes (comment order,
# format name) are considered the same...
add_test(NAME rinheaddiff_2OBS_Diff_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DFILE1=RINEX2_OBS_ref.txt
         -DFILE2=RINEX2_OBS_1.txt
         -DTESTBASE=rinheaddiff_2obs_1
         -DSOURCEDIR=${SD}/rinheaddiff
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinheadsame.cmake)

# Make sure differences are recognized
add_test(NAME rinheaddiff_2OBS_Diff_2a
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DFILE1=RINEX2_OBS_ref.txt
         -DFILE2=RINEX2_OBS_2.txt
         -DTESTBASE=rinheaddiff_2obs_2a
         -DSOURCEDIR=${SD}/rinheaddiff
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinheaddiff.cmake)

# Make sure differences are recognized with one exclusion
set( OPT_2B --exclude=COMMENT )
add_test(NAME rinheaddiff_2OBS_Diff_2b
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DFILE1=RINEX2_OBS_ref.txt
         -DFILE2=RINEX2_OBS_2.txt
         -DIXOPTS=${OPT_2B}
         -DTESTBASE=rinheaddiff_2obs_2b
         -DSOURCEDIR=${SD}/rinheaddiff
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinheaddiff.cmake)

# Make sure differences are recognized with one inclusion
set( OPT_2C --include=COMMENT )
add_test(NAME rinheaddiff_2OBS_Diff_2c
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DFILE1=RINEX2_OBS_ref.txt
         -DFILE2=RINEX2_OBS_2.txt
         -DIXOPTS=${OPT_2C}
         -DTESTBASE=rinheaddiff_2obs_2c
         -DSOURCEDIR=${SD}/rinheaddiff
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinheaddiff.cmake)

# Make sure sameness is recognized with one inclusion
set( OPT_2D "LEAP SECONDS" )
add_test(NAME rinheaddiff_2OBS_Diff_2d
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DFILE1=RINEX2_OBS_ref.txt
         -DFILE2=RINEX2_OBS_2.txt
         -DIXOPTS=--include=${OPT_2D}
         -DTESTBASE=rinheaddiff_2obs_2d
         -DSOURCEDIR=${SD}/rinheaddiff
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinheadsame.cmake)

# Make sure sameness is recognized when excluding all differences.
# Uses a separate script because I can't be bothered to spend any more
# time trying to figure out how to get escaped parameters to the app.
add_test(NAME rinheaddiff_2OBS_Diff_2e
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rinheaddiff>
         -DFILE1=RINEX2_OBS_ref.txt
         -DFILE2=RINEX2_OBS_2.txt
         -DIXOPTS=--include=${OPT_2E}
         -DTESTBASE=rinheaddiff_2obs_2e
         -DSOURCEDIR=${SD}/rinheaddiff
         -DTARGETDIR=${TD}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinhead2e.cmake)
