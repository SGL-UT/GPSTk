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



