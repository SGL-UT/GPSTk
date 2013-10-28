# apps/visibility/CMakeLists.txt

add_library(vislib STATIC
StaStats.cpp
VisSupport.cpp
DiscreteVisibleCounts.cpp
)
target_link_libraries(vislib)

add_executable(wheresat WhereSat.cpp)
target_link_libraries(wheresat gpstk)
install (TARGETS wheresat DESTINATION bin)

add_executable(svvis svvis.cpp)
target_link_libraries(svvis gpstk)
install (TARGETS svvis DESTINATION bin)

add_executable(findMoreThan12 findMoreThan12.cpp)
target_link_libraries(findMoreThan12 gpstk)
install (TARGETS findMoreThan12 DESTINATION bin)

add_executable(compSatVis compSatVis.cpp)
target_link_libraries(compSatVis gpstk vislib)
install (TARGETS compSatVis DESTINATION bin)

add_executable(compStaVis compStaVis.cpp)
target_link_libraries(compStaVis gpstk vislib)
install (TARGETS compStaVis DESTINATION bin)

add_executable(DOPcalc DOPcalc.cpp)
target_link_libraries(DOPcalc gpstk)
install (TARGETS DOPcalc DESTINATION bin)

add_executable(CalcDOPs CalcDOPs.cpp)
target_link_libraries(CalcDOPs gpstk)
install (TARGETS CalcDOPs DESTINATION bin)

