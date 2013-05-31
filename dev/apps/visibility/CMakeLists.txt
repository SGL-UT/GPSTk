# apps/visibility/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/rxio
)

add_library(vislib STATIC
StaStats.cpp
VisSupport.cpp
DiscreteVisibleCounts.cpp
)
target_link_libraries(vislib gpstk)

add_executable(wheresat WhereSat.cpp)
target_link_libraries(wheresat gpstk rxio)

add_executable(svvis svvis.cpp)
target_link_libraries(svvis gpstk rxio)

add_executable(findMoreThan12 findMoreThan12.cpp)
target_link_libraries(findMoreThan12 gpstk rxio)

add_executable(compSatVis compSatVis.cpp)
target_link_libraries(compSatVis gpstk rxio vislib)

add_executable(compStaVis compStaVis.cpp)
target_link_libraries(compStaVis gpstk rxio vislib)

add_executable(DOPcalc DOPcalc.cpp)
target_link_libraries(DOPcalc gpstk rxio)

add_executable(CalcDOPs CalcDOPs.cpp)
target_link_libraries(CalcDOPs gpstk rxio)

