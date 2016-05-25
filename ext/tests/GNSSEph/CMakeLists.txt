
add_executable(OrbSysStore_T OrbSysStore_T.cpp)
target_link_libraries(OrbSysStore_T gpstk)
add_test(GNSSEph_OrbSysStore OrbSysStore_T)
set_property(TEST GNSSEph_OrbSysStore PROPERTY LABELS GNSSEph OrbSysStore)

