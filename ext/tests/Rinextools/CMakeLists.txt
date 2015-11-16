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
