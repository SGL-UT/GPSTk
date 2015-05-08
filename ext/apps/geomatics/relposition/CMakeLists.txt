# apps/geomatics/relposition/CMakeLists.txt

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
target_link_libraries(baselib gpstk)

add_executable(DDBase DDBase.cpp)
target_link_libraries(DDBase baselib)
install (TARGETS DDBase DESTINATION "${CMAKE_INSTALL_BINDIR}")



