# apps/reszilla/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/rxio
${CMAKE_SOURCE_DIR}/lib/geomatics
)

add_library(rlib STATIC
DDEpoch.cpp
PhaseCleaner.cpp
PhaseResidual.cpp
RobustLinearEstimator.cpp
SvElevationMap.cpp
CycleSlipList.cpp
OrdApp.cpp
OrdEngine.cpp
)
target_link_libraries(rlib rxio geomatics)


add_executable(ordGen ordGen.cpp)
target_link_libraries(ordGen rlib)
install (TARGETS ordGen DESTINATION bin)

add_executable(ordClock ordClock.cpp)
target_link_libraries(ordClock rlib)
install (TARGETS ordClock DESTINATION bin)

add_executable(ordLinEst ordLinEst.cpp)
target_link_libraries(ordLinEst rlib)
install (TARGETS ordLinEst DESTINATION bin)

add_executable(ordEdit ordEdit.cpp)
target_link_libraries(ordEdit rlib)
install (TARGETS ordEdit DESTINATION bin)

add_executable(ordStats ordStats.cpp)
target_link_libraries(ordStats rlib)
install (TARGETS ordStats DESTINATION bin)

add_executable(ddGen ddGen.cpp)
target_link_libraries(ddGen rlib)
install (TARGETS ddGen DESTINATION bin)

install(PROGRAMS ordPlot ddPlot.py ddrecord.py DESTINATION bin)