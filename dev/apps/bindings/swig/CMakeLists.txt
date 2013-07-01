# this file is not added to parent cmakelists.txt files yet
cmake_minimum_required(VERSION 2.8)

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# warning disables:
# 362: operator= ignored
# 383: operator++ ignored
# 384: operator-- ignored
# 503: can't wrap 'identifer' unless renamed to a valid identifier (e.g. operator<<)
set(CMAKE_SWIG_FLAGS "-w362,383,384,503")
set(CMAKE_CXX_FLAGS "-std=c++11")

set_source_files_properties(gpstk.i PROPERTIES CPLUSPLUS ON)
swig_add_module(gpstk python gpstk.i
					../../../src/AlmOrbit.cpp
					../../../src/ANSITime.cpp
					../../../src/BaseDistribution.hpp
					../../../src/BinUtils.cpp
					../../../src/BrcClockCorrection.cpp
					../../../src/BrcKeplerOrbit.cpp
					../../../src/Chi2Distribution.cpp
					../../../src/CivilTime.cpp
					../../../src/ClockSatStore.cpp
					../../../src/CommonTime.cpp
					../../../src/convhelp.hpp
					../../../src/EllipsoidModel.hpp
					../../../src/EngAlmanac.cpp
					../../../src/EngEphemeris.cpp
					../../../src/EngNav.cpp
					../../../src/Exception.cpp
					../../../src/FFData.cpp
					../../../src/FFStream.cpp
					../../../src/FFStreamError.hpp
					../../../src/FFTextStream.hpp
					../../../src/FileStore.hpp
					../../../src/GalEphemeris.cpp
					../../../src/GalEphemerisStore.cpp
					../../../src/GaussianDistribution.cpp
					../../../src/geometry.hpp
					../../../src/GNSSconstants.hpp
					../../../src/gps_constants.hpp
					../../../src/GPS_URA.hpp
					../../../src/GPSAlmanacStore.cpp
					../../../src/GPSEphemerisStore.cpp
					../../../src/gpstkplatform.h
					../../../src/GPSWeek.cpp
           			../../../src/GPSWeekZcount.cpp
					../../../src/GPSWeekSecond.cpp
					../../../src/GPSZcount.cpp
					../../../src/JulianDate.cpp
					../../../src/MathBase.hpp
					../../../src/MJD.cpp
					../../../src/ObsID.cpp
					../../../src/ObsID.hpp
					../../../src/ObsIDInitializer.cpp
					../../../src/OrbElemStore.hpp
					../../../src/Position.cpp
					../../../src/PositionSatStore.cpp
					../../../src/PZ90Ellipsoid.hpp
					../../../src/ReferenceFrame.cpp
					../../../src/Rinex3ClockBase.hpp
					../../../src/Rinex3ClockData.cpp
					../../../src/Rinex3ClockHeader.cpp
					../../../src/Rinex3ObsHeader.cpp
					../../../src/Rinex3ObsHeader.hpp
					../../../src/Rinex3ObsStream.hpp
					../../../src/RinexClockBase.hpp
					../../../src/RinexClockData.cpp
					../../../src/RinexClockHeader.cpp
					../../../src/RinexObsHeader.cpp
					../../../src/RinexObsHeader.hpp
					../../../src/RinexObsID.cpp
					../../../src/RinexObsStream.hpp
					../../../src/RinexSatID.cpp
					../../../src/SatID.hpp
					../../../src/SatID.hpp
					../../../src/SP3Base.hpp
					../../../src/SP3Data.cpp
					../../../src/SP3EphemerisStore.cpp
					../../../src/SP3Header.cpp
					../../../src/SP3SatID.cpp
					../../../src/SpecialFunctions.cpp
					../../../src/Stats.hpp
					../../../src/StringUtils.hpp
					../../../src/StudentDistribution.cpp
					../../../src/SVNumXRef.cpp
					../../../src/SystemTime.cpp
					../../../src/TabularSatStore.hpp
					../../../src/TimeConstants.hpp
					../../../src/TimeConverters.cpp
					../../../src/TimeString.cpp
					../../../src/TimeSystem.cpp
					../../../src/TimeTag.cpp
					../../../src/TimeTag.hpp
					../../../src/Triple.cpp
					../../../src/UnixTime.cpp
					../../../src/UnixTime.cpp
					../../../src/Vector.hpp
					../../../src/VectorBase.hpp
					../../../src/WGS84Ellipsoid.hpp
					../../../src/Xv.hpp
					../../../src/Xvt.cpp
					../../../src/XvtStore.hpp
					../../../src/YDSTime.cpp
					../../../src/YumaAlmanacStore.cpp
					../../../src/YumaData.cpp
					../../../src/YumaHeader.hpp
					)

swig_link_libraries(gpstk ${PYTHON_LIBRARIES})
