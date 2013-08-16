# lib/rxio/CMakeLists.txt
set (rxio_src
AshtechALB.cpp 
AshtechData.cpp 
AshtechEPB.cpp 
AshtechMBEN.cpp 
AshtechPBEN.cpp
ATSData.cpp 
EphReader.cpp 
FFIdentifier.cpp 
MDPHeader.cpp
MDPNavSubframe.cpp 
MDPObsEpoch.cpp 
MDPPVTSolution.cpp
MDPSelftestStatus.cpp 
MetReader.cpp 
NovatelData.cpp 
ObsEphReaderFramework.cpp
ObsReader.cpp
ObsUtils.cpp
RinexConverters.cpp
)
   
add_library (rxio ${STADYN} ${rxio_src})
include_directories (${CMAKE_SOURCE_DIR}/src)
install (DIRECTORY . DESTINATION include/gpstk FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
target_link_libraries (rxio gpstk)
install (TARGETS rxio DESTINATION lib)
