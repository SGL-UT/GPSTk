
add_executable(OrbSysStore_T OrbSysStore_T.cpp)
target_link_libraries(OrbSysStore_T gpstk)
add_test(GNSSEph_OrbSysStore OrbSysStore_T)
set_property(TEST GNSSEph_OrbSysStore PROPERTY LABELS GNSSEph OrbSysStore)

add_executable(OrbAlmStore_T OrbAlmStore_T.cpp)
target_link_libraries(OrbAlmStore_T gpstk)
add_test(GNSSEph_OrbAlmStore OrbAlmStore_T)
set_property(TEST GNSSEph_OrbAlmStore PROPERTY LABELS GNSSEph OrbAlmStore)

# Need to link the test program.  However, instead of running
# tests that lead to internal pass/fail, this program produces
# a text file that needs to be compared against truth data.
add_executable(OrbAlm_T OrbAlm_T.cpp)
target_link_libraries(OrbAlm_T gpstk)
add_test(NAME OrbAlm_T_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:OrbAlm_T>
         -DTESTBASE=OrbAlm_T_1
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=test_output_OrbAlm_T_GPS_LNAV
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsame.cmake)

#add_test(GNSSEph_OrbAlm OrbAlm_T)
#set_property(TEST GNSSEph_OrbAlm PROPERTY LABELS GNSSEph OrbAlm)

# Need to link the test program.  However, instead of running
# tests that lead to internal pass/fail, this program produces
# a text file that needs to be compared against truth data.
#
# First run the test for LNAV, then run the test for CNAV
#
add_executable(OrbDataSys_T OrbDataSys_T.cpp)
target_link_libraries(OrbDataSys_T gpstk)
add_test(NAME OrbDataSys_T_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:OrbDataSys_T>
         -DARGS=LNAV
         -DTESTBASE=OrbDataSys_T_1
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=test_output_OrbDataSys_T_GPS_LNAV
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsame.cmake)
add_test(NAME OrbDataSys_T_2
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:OrbDataSys_T>
         -DARGS=CNAV
         -DTESTBASE=OrbDataSys_T_2
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=test_output_OrbDataSys_T_GPS_CNAV
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsame.cmake)
#
#  Following tests the get*() methods of various CNAV MT classes
add_executable(OrbSysGpsC_T OrbSysGpsC_T.cpp)
target_link_libraries(OrbSysGpsC_T gpstk)
add_test(GNSSEph_OrbSysGpsC OrbSysGpsC_T)
set_property(TEST GNSSEph_OrbSysGpsC PROPERTY LABELS GNSSEph OrbSysGpsC)
