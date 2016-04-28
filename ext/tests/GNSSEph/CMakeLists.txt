
add_executable(OrbDataSys_T OrbDataSys_T.cpp)
target_link_libraries(OrbDataSys_T gpstk)
add_test(GNSSEph_OrbDataSys OrbDataSys_T)
set_property(TEST GNSSEph_OrbDataSys PROPERTY LABELS GNSSEph OrbDataSys)

