# lib/geomatics/CMakeLists.txt
set (geomatics_src
AntexData.cpp
AntexHeader.cpp
DDid.cpp 
DiscCorr.cpp 
EarthOrientation.cpp 
format.cpp
GeodeticFrames.cpp 
GSatID.cpp 
Namelist.cpp
PhaseWindup.cpp 
PreciseRange.cpp 
random.cpp
RobustStats.cpp 
SatPass.cpp 
SolarPosition.cpp 
SolarSystem.cpp
SolidEarthTides.cpp
SRI.cpp 
SRIFilter.cpp 
SRIleastSquares.cpp
SunEarthSatGeometry.cpp
)

add_library (geomatics SHARED ${geomatics_src})
include_directories (${CMAKE_SOURCE_DIR}/src)
target_link_libraries (geomatics gpstk)
