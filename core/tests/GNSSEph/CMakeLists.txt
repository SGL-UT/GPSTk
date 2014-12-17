#Tests for GNSSEph Classes

add_executable(AlmOrbit_T AlmOrbit_T.cpp)
target_link_libraries(AlmOrbit_T gpstk)
add_test(GNSSEph_AlmOrbit* AlmOrbit_T)

add_executable(EngAlmanac_T EngAlmanac_T.cpp)
target_link_libraries(EngAlmanac_T gpstk)
add_test(GNSSEph_EngAlmanac* EngAlmanac_T)

add_executable(EngEphemeris_T EngEphemeris_T.cpp)
target_link_libraries(EngEphemeris_T gpstk)
add_test(GNSSEph_EngEphemeris* EngEphemeris_T)

add_executable(EngNav_T EngNav_T.cpp)
target_link_libraries(EngNav_T gpstk)
add_test(GNSSEph_EngNav* EngNav_T)

add_executable(EphemerisRange_T EphemerisRange_T.cpp)
target_link_libraries(EphemerisRange_T gpstk)
add_test(GNSSEph_EphemerisRange* EphemerisRange_T)

#add_executable(RinexEphemerisStore_T RinexEphemerisStore_T.cpp ftest.cpp)
#target_link_libraries(RinexEphemerisStore_T gpstk)
#add_test(GNSSEph_RinexEphemerisStore RinexEphemerisStore_T)

#add_executable(SP3EphemerisStore_T SP3EphemerisStore_T.cpp ftest.cpp)
#target_link_libraries(SP3EphemerisStore_T gpstk)
#add_test(GNSSEph_SP3EphemerisStore SP3EphemerisStore_T)

add_executable(SP3SatID_T SP3SatID_T.cpp)
target_link_libraries(SP3SatID_T gpstk)
add_test(GNSSEph_SP3SatID* SP3SatID_T)