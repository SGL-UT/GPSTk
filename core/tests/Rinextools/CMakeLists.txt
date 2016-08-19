# core/tests/Rinextools

set( TD ${GPSTK_TEST_OUTPUT_DIR} )
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


###############################################################################
# TEST RinEdit
###############################################################################

# Header change test
add_test(NAME RinEdit_Header
    COMMAND ${CMAKE_COMMAND}
    -DTEST_PROG=$<TARGET_FILE:RinEdit>
    -DTESTBASE=RinEdit_Header
    -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
    -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
    -DARGS=--IF\ ${GPSTK_TEST_DATA_DIR}/arlm200a.15o\ --OF\ ${TD}/RinEdit_Header.out\ --HDr\ RIGTC\ --HDx\ 3979316.4389,1050312.2534,4857066.9036\ --HDn\ 11502M002\ --HDm\ GOPE\ --HDa\ RIGTC\ GO\ PECNY
    -DOWNOUTPUT=1
    -DDIFF_PROG=$<TARGET_FILE:df_diff>
    -DDIFF_ARGS=-l2
    -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Test that the output of RinEdit_Header is a valid Rinex Obs file.
add_test(NAME RinEdit_ValidOutput
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:rowcheck>
         -DARGS=${TD}/RinEdit_Header.out
         -DTESTBASE=RinEdit_ValidOutput
         -DTARGETDIR=${TD}
         -DNODIFF=1
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)
set_tests_properties(RinEdit_ValidOutput PROPERTIES DEPENDS RinEdit_Header)


###############################################################################
# TEST RinSum
###############################################################################

# Check RinSum output with --ycode option
add_test(NAME RinSum_ycode
    COMMAND ${CMAKE_COMMAND}
    -DTEST_PROG=$<TARGET_FILE:RinSum>
    -DTESTBASE=RinSum_ycode
    -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
    -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
    -DARGS=--ycode\ --obs\ ${GPSTK_TEST_DATA_DIR}/arlm200a.15o\ --quiet
    -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake)

# Check RinSum with obspath and Rinex v2.11 file
add_test(NAME RinSum_obspath_v211
    COMMAND ${CMAKE_COMMAND}
    -DTEST_PROG=$<TARGET_FILE:RinSum>
    -DDIFF_PROG=$<TARGET_FILE:df_diff>
    -DTESTBASE=RinSum_obspath_v211
    -DARGS=--obspath\ ${GPSTK_TEST_DATA_DIR}/inputs/igs\ --obs\ faa1170b00.16o
    -DDIFF_ARGS=-l\ 2\ -v
    -DTARGETDIR=${TD}
    -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}/outputs
    -P ${CMAKE_CURRENT_SOURCE_DIR}/../testsuccexp.cmake
    )
