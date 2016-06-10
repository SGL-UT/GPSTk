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



