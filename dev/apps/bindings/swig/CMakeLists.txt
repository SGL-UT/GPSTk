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
# 503: can't wrap 'identifer' unless renamed to a valid identifier
set(CMAKE_SWIG_FLAGS "-w362,383,384,503")
set(CMAKE_CXX_FLAGS "-std=c++11")

set_source_files_properties(gpstk.i PROPERTIES CPLUSPLUS ON) 
swig_add_module(gpstk python gpstk.i 
					../../../src/AlmOrbit.cpp
					../../../src/ANSITime.cpp
					../../../src/BinUtils.cpp
					../../../src/BrcClockCorrection.cpp
					../../../src/BrcKeplerOrbit.cpp
					../../../src/CivilTime.cpp
					../../../src/CommonTime.cpp
					../../../src/convhelp.hpp
					../../../src/EllipsoidModel.hpp
					../../../src/EngAlmanac.cpp
					../../../src/EngEphemeris.cpp
					../../../src/EngNav.cpp
					../../../src/Exception.cpp
					../../../src/Exception.cpp
					../../../src/Exception.cpp
					../../../src/FFData.cpp
					../../../src/FFStream.cpp
					../../../src/FFStreamError.hpp
					../../../src/geometry.hpp
					../../../src/GNSSconstants.hpp
					../../../src/gps_constants.hpp
					../../../src/GPS_URA.hpp
					../../../src/GPSAlmanacStore.cpp
					../../../src/GPSEphemerisStore.cpp
					../../../src/gpstkplatform.h
					../../../src/GPSWeek.cpp
					../../../src/GPSWeek.cpp
					../../../src/GPSWeekSecond.cpp
					../../../src/GPSWeekZcount.cpp
					../../../src/GPSZcount.cpp
					../../../src/JulianDate.cpp
					../../../src/JulianDate.cpp
					../../../src/MathBase.hpp
					../../../src/MJD.cpp
					../../../src/ObsID.cpp
					../../../src/ObsID.cpp
					../../../src/ObsID.hpp
					../../../src/ObsIDInitializer.cpp
					../../../src/OrbElemStore.hpp
					../../../src/Position.cpp
					../../../src/PZ90Ellipsoid.hpp
					../../../src/ReferenceFrame.cpp
					../../../src/ReferenceFrame.cpp
					../../../src/RinexObsHeader.cpp
					../../../src/RinexSatID.cpp
					../../../src/SatID.hpp
					../../../src/SatID.hpp
					../../../src/SpecialFunctions.cpp
					../../../src/StringUtils.hpp
					../../../src/StringUtils.hpp
					../../../src/SVNumXRef.cpp
					../../../src/SystemTime.cpp
					../../../src/SystemTime.cpp
					../../../src/TimeConstants.hpp
					../../../src/TimeConverters.cpp
					../../../src/TimeString.cpp
					../../../src/TimeSystem.cpp
					../../../src/TimeTag.cpp
					../../../src/TimeTag.cpp
					../../../src/TimeTag.hpp
					../../../src/Triple.cpp
					../../../src/UnixTime.cpp
					../../../src/UnixTime.cpp
					../../../src/WGS84Ellipsoid.hpp
					../../../src/Xv.hpp
					../../../src/Xvt.cpp
					../../../src/XvtStore.hpp
					../../../src/YDSTime.cpp
					../../../src/YumaAlmanacStore.cpp
					../../../src/YumaData.cpp
					)

swig_link_libraries(gpstk ${PYTHON_LIBRARIES})
