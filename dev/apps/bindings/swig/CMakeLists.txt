# this file is not added to parent cmakelists.txt files
cmake_minimum_required(VERSION 2.8)

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
message(STATUS "Building using swig at: ${SWIG_EXECUTABLE}")

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../../../src/)
message(STATUS "Building against python include path: ${PYTHON_INCLUDE_PATH}")
message(STATUS "Building against python library: ${PYTHON_LIBRARIES}")

# warning disables:
# 362: operator= ignored
# 383: operator++ ignored
# 384: operator-- ignored
# 389: operator[] ignored
# 503: can't wrap 'identifer' unless renamed to a valid identifier (e.g. operator<<)
set(CMAKE_SWIG_FLAGS "-w362,383,384,389,503")
set(CMAKE_CXX_FLAGS "-std=c++11")
set_source_files_properties(gpstk_pylib.i PROPERTIES CPLUSPLUS ON)
swig_add_module(gpstk_pylib python gpstk_pylib.i
                    ../../../src/AlmOrbit.cpp
                    ../../../src/ANSITime.cpp
                    ../../../src/Antenna.cpp
                    ../../../src/AstronomicalFunctions.cpp
                    ../../../src/BDSEphemeris.cpp
                    ../../../src/BDSEphemerisStore.cpp
                    ../../../src/BinUtils.cpp
                    ../../../src/BrcClockCorrection.cpp
                    ../../../src/BrcKeplerOrbit.cpp
                    ../../../src/CivilTime.cpp
                    ../../../src/ClockSatStore.cpp
                    ../../../src/CommonTime.cpp
                    ../../../src/EngAlmanac.cpp
                    ../../../src/EngEphemeris.cpp
                    ../../../src/EngNav.cpp
                    ../../../src/Exception.cpp
                    ../../../src/Expression.cpp
                    ../../../src/ExtractData.cpp
                    ../../../src/FFData.cpp
                    ../../../src/FFStream.cpp
                    ../../../src/FICData.cpp
                    ../../../src/FICHeader.cpp
                    ../../../src/GalEphemeris.cpp
                    ../../../src/GalEphemerisStore.cpp
                    ../../../src/GloEphemeris.cpp
                    ../../../src/GloEphemerisStore.cpp
                    ../../../src/GPSAlmanacStore.cpp
                    ../../../src/GPSEphemeris.cpp
                    ../../../src/GPSEphemerisStore.cpp
                    ../../../src/gpstkplatform.h
                    ../../../src/GPSWeek.cpp
                    ../../../src/GPSWeekZcount.cpp
                    ../../../src/GPSZcount.cpp
                    ../../../src/JulianDate.cpp
                    ../../../src/MJD.cpp
                    ../../../src/MoonPosition.cpp
                    ../../../src/MSCData.cpp
                    ../../../src/MSCStore.cpp
                    ../../../src/ObsEpochMap.cpp
                    ../../../src/ObsID.cpp
                    ../../../src/ObsIDInitializer.cpp
                    ../../../src/OrbElem.cpp
                    ../../../src/OrbElemRinex.cpp
                    ../../../src/OrbitEph.cpp
                    ../../../src/OrbitEphStore.cpp
                    ../../../src/PoleTides.cpp
                    ../../../src/Position.cpp
                    ../../../src/PositionSatStore.cpp
                    ../../../src/PRSolution2.cpp
                    ../../../src/QZSEphemeris.cpp
                    ../../../src/QZSEphemerisStore.cpp
                    ../../../src/ReferenceFrame.cpp
                    ../../../src/Rinex3ClockData.cpp
                    ../../../src/Rinex3ClockHeader.cpp
                    ../../../src/Rinex3EphemerisStore.cpp
                    ../../../src/Rinex3NavData.cpp
                    ../../../src/Rinex3NavHeader.cpp
                    ../../../src/Rinex3ObsData.cpp
                    ../../../src/Rinex3ObsHeader.cpp
                    ../../../src/RinexClockData.cpp
                    ../../../src/RinexClockHeader.cpp
                    ../../../src/RinexEphemerisStore.cpp
                    ../../../src/RinexMetData.cpp
                    ../../../src/RinexMetHeader.cpp
                    ../../../src/RinexNavData.cpp
                    ../../../src/RinexNavHeader.cpp
                    ../../../src/RinexObsData.cpp
                    ../../../src/RinexObsHeader.cpp
                    ../../../src/RinexObsID.cpp
                    ../../../src/RinexSatID.cpp
                    ../../../src/RinexUtilities.cpp
                    ../../../src/SEMAlmanacStore.cpp
                    ../../../src/SEMData.cpp
                    ../../../src/SEMHeader.cpp
                    ../../../src/SolidTides.cpp
                    ../../../src/SP3Data.cpp
                    ../../../src/SP3EphemerisStore.cpp
                    ../../../src/SP3Header.cpp
                    ../../../src/SP3SatID.cpp
                    ../../../src/SunPosition.cpp
                    ../../../src/SVNumXRef.cpp
                    ../../../src/SystemTime.cpp
                    ../../../src/TimeConverters.cpp
                    ../../../src/TimeString.cpp
                    ../../../src/TimeSystem.cpp
                    ../../../src/TimeTag.cpp
                    ../../../src/Triple.cpp
                    ../../../src/TropModel.cpp
                    ../../../src/UnixTime.cpp
                    ../../../src/Week.cpp
                    ../../../src/WeekSecond.cpp
                    ../../../src/WxObsMap.cpp
                    ../../../src/Xvt.cpp
                    ../../../src/YDSTime.cpp
                    ../../../src/YumaAlmanacStore.cpp
                    ../../../src/YumaData.cpp
                    )
