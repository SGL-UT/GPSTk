
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
add_executable(OrbDataSys_T OrbDataSys_T.cpp)
target_link_libraries(OrbDataSys_T gpstk)
add_test(NAME OrbDataSys_T_1
         COMMAND ${CMAKE_COMMAND}
         -DTEST_PROG=$<TARGET_FILE:OrbDataSys_T>
         -DTESTBASE=OrbDataSys_T_1
         -DSOURCEDIR=${GPSTK_TEST_DATA_DIR}
         -DTARGETDIR=${GPSTK_TEST_OUTPUT_DIR}
         -DTESTBASE=test_output_OrbDataSys_T_GPS_LNAV
         -P ${CMAKE_CURRENT_SOURCE_DIR}/testsame.cmake)
