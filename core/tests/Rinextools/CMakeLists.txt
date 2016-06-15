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

# Test no Sat (default) with C1C data
set( ARGS1 ${GPSTK_TEST_DATA_DIR}/test_input_rinex3_obs_RinexObsFile.15o\ C1C\ )
add_test(NAME RinDump_Default_C1C
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinDump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=RinDump_sat_default_C1C
         -DARGS=${ARGS1}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsuccdiff_RinDump.cmake)

# Test Sat with C1C data
set( ARGS2 ${GPSTK_TEST_DATA_DIR}/test_input_rinex3_obs_RinexObsFile.15o\ G05\ C1C\ )
add_test(NAME RinDump_Single_C1C
         COMMAND ${CMAKE_COMMAND} 
         -DTEST_PROG=$<TARGET_FILE:RinDump>
         -DDIFF_PROG=$<TARGET_FILE:df_diff>
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=RinDump_sat_single_C1C
         -DARGS=${ARGS2}   	    
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsuccdiff_RinDump.cmake)

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
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsuccdiff_RinDump.cmake)

# Check that a missing Rinex obs file results in failure
add_test(NAME RinDump_MissingInputRObs
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinDump>
         -DARGS=--obs\ ${GPSTK_TEST_DATA_DIR}/notAFile.txt\ --sat\ G05\ --dat\ C1C
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# Check that a missing Rinex nav file results in failure
add_test(NAME RinDump_MissingInputRNav
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinDump>
         -DARGS=--obs\ ${GPSTK_TEST_DATA_DIR}/arlm200a.15o\ --sat\ G05\ --dat\ C1C\ --nav\ ${GPSTK_TEST_DATA_DIR}/notAFile.txt
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# Check that a missing SP3 file results in failure
add_test(NAME RinDump_MissingInputSP3
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinDump>
         -DARGS=--obs\ ${GPSTK_TEST_DATA_DIR}/arlm200a.15o\ --sat\ G05\ --dat\ C1C\ --eph\ ${GPSTK_TEST_DATA_DIR}/notAFile.txt
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# Check that an invalid Rinex obs file results in failure
add_test(NAME RinDump_InvalidInputRObs
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinDump>
         -DARGS=--obs\ ${GPSTK_TEST_DATA_DIR}/timeconvert_2015_200_99.exp\ --sat\ G05\ --dat\ C1C
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# Check that an invalid Rinex nav file results in failure
add_test(NAME RinDump_InvalidInputRNav
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinDump>
         -DARGS=--obs\ ${GPSTK_TEST_DATA_DIR}/arlm200a.15o\ --sat\ G05\ --dat\ C1C\ --nav\ ${GPSTK_TEST_DATA_DIR}/timeconvert_2015_200_99.exp
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# Check that an invalid SP3 file results in failure
add_test(NAME RinDump_InvalidInputSP3
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinDump>
         -DARGS=--obs\ ${GPSTK_TEST_DATA_DIR}/arlm200a.15o\ --sat\ G05\ --dat\ C1C\ --eph\ ${GPSTK_TEST_DATA_DIR}/timeconvert_2015_200_99.exp
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

###############################################################################
# TEST RinEdit
###############################################################################

# Header change test
add_test(NAME RinEdit_Header
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinEdit>
         -DRINHEADDIFF=$<TARGET_FILE:rinheaddiff>
         -DROWDIFF=$<TARGET_FILE:rowdiff>
         -DINFILE=arlm200a.15o
         -DTESTBASE=RinEdit_Header
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinheader.cmake)

# Check that a missing input file results in failure
add_test(NAME RinEdit_MissingInput
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinEdit>
         -DARGS=--IF\ ${GPSTK_TEST_DATA_DIR}/notAFile.txt\ --OF ${GPSTK_TEST_OUTPUT_DIR}/RinEdit_MissingInput.out
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# Check that an invalid input file results in failure
add_test(NAME RinEdit_InvalidInput
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinEdit>
         -DARGS=--IF\ ${GPSTK_TEST_DATA_DIR}/timeconvert_2015_200_99.exp\ --OF ${GPSTK_TEST_OUTPUT_DIR}/RinEdit_InvalidInput.out
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# Take a valid Rinex file, do nothing, and ensure the printout is still RINEX
add_test(NAME RinEdit_ValidOutput
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinEdit>
         -DARGS=--IF\ ${GPSTK_TEST_DATA_DIR}/arlm200a.15o\ --OF\ ${GPSTK_TEST_OUTPUT_DIR}/RinEdit_ValidOutput.out
         -DTESTBASE=RinEdit_ValidOutput
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DGPSTK_BINDIR=${PROJECT_BINARY_DIR}/core/apps/checktools/
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testValidOutput_RObs.cmake)
###############################################################################
# TEST RinSum
###############################################################################

# Check RinSum output with --ycode option
add_test(NAME RinSum_ycode
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinSum>
         -DINFILE=arlm200a.15o
         -DTESTBASE=RinSum_ycode
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DOPTS=--ycode
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testrinsum.cmake)

# Check that a missing input file results in failure
add_test(NAME RinSum_MissingInput
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinSum>
         -DARGS=--obs\ ${GPSTK_TEST_DATA_DIR}/notAFile.txt
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

# Check that an invalid input file results in failure
add_test(NAME RinSum_InvalidInput
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:RinSum>
         -DARGS=--obs\ ${GPSTK_TEST_DATA_DIR}/timeconvert_2015_200_99.exp
         -P ${CMAKE_CURRENT_SOURCE_DIR}/../testfailexp.cmake)

