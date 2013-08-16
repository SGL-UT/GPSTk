# apps/geomatics/relposition/CMakeLists.txt

include_directories(
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib/geomatics
)

add_library(baselib STATIC
DataStructures.cpp
CommandInput.cpp
Configure.cpp
ReadRawData.cpp
ReadObsFiles.cpp
EphemerisImprovement.cpp
ClockModel.cpp
Synchronization.cpp
DoubleDifference.cpp
EditDDs.cpp
DataOutput.cpp
Estimation.cpp
Timetable.cpp
ElevationMask.cpp
ProcessRawData.cpp
ComputeRAIMSolution.cpp
EditRawDataBuffers.cpp
StochasticModels.cpp
)
target_link_libraries(baselib gpstk geomatics)

add_executable(DDBase DDBase.cpp)
target_link_libraries(DDBase geomatics gpstk baselib)
install (TARGETS DDBase DESTINATION bin)

add_executable(ddmerge ddmerge.cpp)
target_link_libraries(ddmerge baselib gpstk geomatics)
install (TARGETS ddmerge DESTINATION bin)

