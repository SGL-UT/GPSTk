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
target_link_libraries(rlib gpstk rxio geomatics)


add_executable(ordGen ordGen.cpp)
target_link_libraries(ordGen gpstk rxio gpstk rlib)

add_executable(ordClock ordClock.cpp)
target_link_libraries(ordClock gpstk rxio gpstk rlib)

add_executable(ordLinEst ordLinEst.cpp)
target_link_libraries(ordLinEst gpstk rxio gpstk rlib)

add_executable(ordEdit ordEdit.cpp)
target_link_libraries(ordEdit gpstk rxio gpstk rlib)

add_executable(ordStats ordStats.cpp)
target_link_libraries(ordStats gpstk rxio gpstk rlib)

add_executable(ddGen ddGen.cpp)
target_link_libraries(ddGen gpstk rxio gpstk rlib)

install(PROGRAMS ordplot ddPlot.py ddrecord.py
  DESTINATION bin
)