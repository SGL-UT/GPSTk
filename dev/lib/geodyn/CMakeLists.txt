# lib/geodyn/CMakeLists.txt
set (geodyn_src
AtmosphericDrag.cpp
CiraExponentialDrag.cpp
EarthBody.cpp
EarthOceanTide.cpp
EarthPoleTide.cpp
EarthSolidTide.cpp
EGM96GravityModel.cpp
ForceModelList.cpp
HarrisPriesterDrag.cpp
IERS.cpp
IERSConventions.cpp
JGM3GravityModel.cpp
KeplerOrbit.cpp
LEOSatOrbit.cpp
Msise00Drag.cpp
MoonForce.cpp
NavSatOrbit.cpp
PvtStore.cpp
ReferenceFrames.cpp
RelativityEffect.cpp
RungeKuttaFehlberg.cpp 
SatOrbit.cpp
SatOrbitPropagator.cpp
SatOrbitStore.cpp
Spacecraft.cpp
SphericalHarmonicGravity.cpp
SolarRadiationPressure.cpp
SunForce.cpp
UTCTime.cpp
)

add_library (geodyn SHARED ${geodyn_src})
include_directories (${CMAKE_SOURCE_DIR}/lib/geomatics ${CMAKE_SOURCE_DIR}/src)
target_link_libraries (geodyn geomatics)
