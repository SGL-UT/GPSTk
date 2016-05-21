#Tests for GNSSEph Classes

add_executable(AlmOrbit_T AlmOrbit_T.cpp)
target_link_libraries(AlmOrbit_T gpstk)
add_test(GNSSEph_AlmOrbit AlmOrbit_T)

add_executable(EngAlmanac_T EngAlmanac_T.cpp)
target_link_libraries(EngAlmanac_T gpstk)
add_test(GNSSEph_EngAlmanac EngAlmanac_T)

add_executable(EngEphemeris_T EngEphemeris_T.cpp)
target_link_libraries(EngEphemeris_T gpstk)
add_test(GNSSEph_EngEphemeris EngEphemeris_T)

add_executable(EngNav_T EngNav_T.cpp)
target_link_libraries(EngNav_T gpstk)
add_test(GNSSEph_EngNav EngNav_T)

add_executable(EphemerisRange_T EphemerisRange_T.cpp)
target_link_libraries(EphemerisRange_T gpstk)
add_test(GNSSEph_EphemerisRange EphemerisRange_T)

add_executable(NavID_T NavID_T.cpp)
target_link_libraries(NavID_T gpstk)
add_test(GNSSEph_NavID NavID_T)

add_executable(PackedNavBits_T PackedNavBits_T.cpp)
target_link_libraries(PackedNavBits_T gpstk)
add_test(GNSSEph_PackedNavBits PackedNavBits_T)
set_property(TEST GNSSEph_PackedNavBits PROPERTY LABELS GNSSEph PackedNavBits)

add_executable(RinexEphemerisStore_T RinexEphemerisStore_T.cpp)
target_link_libraries(RinexEphemerisStore_T gpstk)
add_test(GNSSEph_RinexEphemerisStore RinexEphemerisStore_T)

add_executable(RinexObsID_T RinexObsID_T.cpp)
target_link_libraries(RinexObsID_T gpstk)
add_test(GNSSEph_RinexObsID RinexObsID_T)

add_executable(RinexSatID_T RinexSatID_T.cpp)
target_link_libraries(RinexSatID_T gpstk)
add_test(GNSSEph_RinexSatID RinexSatID_T)

add_executable(SatID_T SatID_T.cpp)
target_link_libraries(SatID_T gpstk)
add_test(GNSSEph_SatID SatID_T)

add_executable(SP3EphemerisStore_T SP3EphemerisStore_T.cpp)
target_link_libraries(SP3EphemerisStore_T gpstk)
add_test(GNSSEph_SP3EphemerisStore SP3EphemerisStore_T)

add_executable(SP3SatID_T SP3SatID_T.cpp)
target_link_libraries(SP3SatID_T gpstk)
add_test(GNSSEph_SP3SatID SP3SatID_T)

add_executable(XvtStore_T XvtStore_T.cpp)
target_link_libraries(XvtStore_T gpstk)
add_test(GNSSEph_XvtStore XvtStore_T)
