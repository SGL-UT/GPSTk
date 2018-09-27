# core/tests/Rinextools

###############################################################################
# TEST RinDump
###############################################################################

# check that -h option is valid
add_test(NAME RinDump_help
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinDump>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testhelp.cmake)

# Test all satellites (default) with C1C data
set( ARGS1 ${GPSTK_TEST_DATA_DIR}/test_input_rinex3_obs_RinexObsFile.15o\ C1C\ )
add_test(NAME RinDump_Default_C1C
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinDump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=RinDump_sat_default_C1C
         -DARGS=${ARGS1}
         -DDIFF_ARGS=-l6\ -z2
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST RinDump_Default_C1C PROPERTY LABELS Rinex3)

# Test single satellite with C1C data
set( ARGS2 ${GPSTK_TEST_DATA_DIR}/test_input_rinex3_obs_RinexObsFile.15o\ G05\ C1C\ )
add_test(NAME RinDump_Single_C1C
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinDump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=RinDump_sat_single_C1C
         -DARGS=${ARGS2}
         -DDIFF_ARGS=-l6\ -z2
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST RinDump_Single_C1C PROPERTY LABELS Rinex3)

# Test Sat with C1C data with RINEX 2 file
set( ARGS2 ${GPSTK_TEST_DATA_DIR}/arlm200a.15o\ G23\ C1C\ )
add_test(NAME RinDump_Rinex2Input
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinDump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=RinDump_Rinex2Input
         -DARGS=${ARGS2}
         -DDIFF_ARGS=-l6\ -z2
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_property(TEST RinDump_Rinex2Input PROPERTY LABELS Rinex2)

test_app_with_stdout(
    RinDump_211Mixed
    RinDump
    Rinex2
    ${GPSTK_TEST_DATA_DIR}/inputs/igs/faa1170b00.16o\ GC1C\ RC1C\ EC1B\ SC5X
    "-l6 -z1")


###############################################################################
# TEST RinEdit
###############################################################################
test_app_with_file(
    RinEdit_Header #test_name
    RinEdit # application name
    Rinex2  # Lables for reporting
    --IF\ ${GPSTK_TEST_DATA_DIR}/arlm200a.15o\ --OF\ ${GPSTK_TEST_OUTPUT_DIR}/RinEdit_Header.out\ --HDr\ RIGTC\ --HDx\ 3979316.4389,1050312.2534,4857066.9036\ --HDn\ 11502M002\ --HDm\ GOPE\ --HDa\ RIGTC\ GO\ PECNY
    -l2 # diff_args
    )

# Test that the output of RinEdit_Header is a valid Rinex Obs file.
add_test(NAME RinEdit_ValidOutput
    COMMAND ${CMAKE_COMMAND}
    -DTEST_PROG=$<TARGET_FILE:rowcheck>
    -DARGS=${GPSTK_TEST_OUTPUT_DIR}/RinEdit_Header.out
    -DTESTBASE=RinEdit_ValidOutput
    -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
    -DNODIFF=1
    -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_tests_properties(RinEdit_ValidOutput PROPERTIES DEPENDS RinEdit_Header)
set_property(TEST RinEdit_ValidOutput PROPERTY LABELS Rinex2)

# Convert 3->2 and keep as much data as possible
test_app_with_file(
    RinEdit_302to211Mixed # test name
    RinEdit # app name
    "Rinex2 Rinex3" # labels
    --ID\ ${GPSTK_TEST_DATA_DIR}/inputs/igs\ --IF\ FAA100PYF_R_20161700100_15M_01S_MO\ --ver2\ --OD\ ${GPSTK_TEST_OUTPUT_DIR}\ --OF\ RinEdit_302to211Mixed.out
    -l2)

# Convert 3->2 and make a GPS only file, uses an input file to configure the editing
test_app_with_file(
    RinEdit_302to211GPS
    RinEdit
    "Rinex2 Rinex3"
    --ID\ ${GPSTK_TEST_DATA_DIR}/inputs/igs\ --IF\ FAA100PYF_R_20161700100_15M_01S_MO\ --ver2\ --OD\ ${GPSTK_TEST_OUTPUT_DIR}\ --OF\ RinEdit_302to211GPS.out\ --file\ ${GPSTK_TEST_DATA_DIR}/inputs/RinEdit_GPS_only
    -l2)

# Edit out the last part of a Rinex3 file
test_app_with_file(
    RinEdit_302split1
    RinEdit
    "Rinex3"
    --IF\ ${GPSTK_TEST_DATA_DIR}/inputs/igs/FAA100PYF_R_20161700100_15M_01S_MO\ --OF\ ${GPSTK_TEST_OUTPUT_DIR}/RinEdit_302split1.out\ --TE\ 2016,6,18,1,0,2
    -l2)

# Edit out the first part of a Rinex3 file
test_app_with_file(
    RinEdit_302split2
    RinEdit
    "Rinex3"
    --IF\ ${GPSTK_TEST_DATA_DIR}/inputs/igs/FAA100PYF_R_20161700100_15M_01S_MO\ --OF\ ${GPSTK_TEST_OUTPUT_DIR}/RinEdit_302split2.out\ --TB\ 2016,6,18,1,0,3
    -l2)

# Combine the previous two files together
test_app_with_file(
    RinEdit_302merge
    RinEdit
    "Rinex3"
    --ID\ ${GPSTK_TEST_OUTPUT_DIR}\ --IF\ RinEdit_302split1.out\ --IF\ RinEdit_302split2.out\ --OF\ ${GPSTK_TEST_OUTPUT_DIR}/RinEdit_302merge.out
    -l2)
set_tests_properties(RinEdit_302merge PROPERTIES DEPENDS "RinEdit_302split1;RinEdit_302split2")


###############################################################################
# TEST RinSum
# Run lots of different source files through RinSum to make sure the readers
# can at least handle the various variations 
###############################################################################

# Check RinSum with obspath and Rinex v2.11 file
test_app_with_stdout(RinSum_obspath_v211 RinSum Rinex2
    --obspath\ ${GPSTK_TEST_DATA_DIR}/inputs/igs\ --obs\ faa1170b00.16o
    "-l2 -v")

# Check RinSum & Rinex v2.10 file
test_app_with_stdout(RinSum_v210_osn3 RinSum Rinex2
    --obs\ ${GPSTK_TEST_DATA_DIR}/inputs/igs/osn31700.16o\ --ycode
    "-l2 -v")

# Check RinSum & Rinex v2.11 file
test_app_with_stdout(RinSum_v211_cags RinSum Rinex2
    --obs\ ${GPSTK_TEST_DATA_DIR}/inputs/igs/cags1700.16o
    "-l2 -v")

# Check RinSum & Rinex v2.11 file
test_app_with_stdout(RinSum_v211_kerg RinSum Rinex2
    --obs\ ${GPSTK_TEST_DATA_DIR}/inputs/igs/kerg1700.16o
    "-l2 -v")

# Check RinSum with Rinex v2.11 file with more obs types
test_app_with_stdout(RinSum_v211_nklg RinSum Rinex2
    --obs\ ${GPSTK_TEST_DATA_DIR}/inputs/igs/nklg170b00.16o
    "-l2 -v")

# Check RinSum Rinex v3.02 input
test_app_with_stdout(RinSum_v302_FAA1 RinSum Rinex3
    --obs\ ${GPSTK_TEST_DATA_DIR}/inputs/igs/FAA100PYF_R_20161700100_15M_01S_MO
    "-l2 -v")

# Check RinSum with Rinex v3.03 input
# Uncomment this when Nathanial's changes make it in
#test_app_with_stdout(RinSum_v302_FAA1 RinSum Rinex3
#    --obs\ ${GPSTK_TEST_DATA_DIR}/inputs/igs/UCAL00CAN_S_20161700100_15M_01S_MO
#    "-l2 -v")


###############################################################################
# TEST scanBrdcFile
###############################################################################
set(test_name scanBrdcFile1)
add_test(NAME ${test_name}
    COMMAND ${CMAKE_COMMAND}
    -DTESTBASE=${test_name}
    -DTEST_PROG=$<TARGET_FILE:scanBrdcFile>
    -DDIFF_PROG=$<TARGET_FILE:df_diff>
    -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
    -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/testScanBrdcFile.cmake)
set_property(TEST  ${test_name} PROPERTY LABELS Rinex2)