# this file is not added to parent cmakelists.txt files
cmake_minimum_required(VERSION 2.8)

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
message(STATUS "Building using swig at: ${SWIG_EXECUTABLE}")

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../../../lib/AstroEph
                    ../../../lib/deprecate
                    ../../../lib/FileDirProc
                    ../../../lib/FileHandling
                    ../../../lib/FileHandling/FIC
                    ../../../lib/FileHandling/MSC
                    ../../../lib/FileHandling/RINEX
                    ../../../lib/FileHandling/RINEX3
                    ../../../lib/FileHandling/SEM
                    ../../../lib/FileHandling/SP3
                    ../../../lib/FileHandling/Yuma
                    ../../../lib/GNSSCore
                    ../../../lib/GNSSEph
                    ../../../lib/Math
                    ../../../lib/Math/Matrix
                    ../../../lib/Math/Vector
                    ../../../lib/Misc
                    ../../../lib/PosSol
                    ../../../lib/ProcLib
                    ../../../lib/RefTime
                    ../../../lib/TimeHandling
                    ../../../lib/Utilities )

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
                    ../../../lib/GNSSEph/AlmOrbit.cpp
                    ../../../lib/TimeHandling/ANSITime.cpp
                    ../../../lib/ProcLib/Antenna.cpp
                    ../../../lib/AstroEph/AstronomicalFunctions.cpp
                    ../../../lib/GNSSEph/BDSEphemeris.cpp
                    ../../../lib/GNSSEph/BDSEphemerisStore.cpp
                    ../../../lib/Utilities/BinUtils.cpp
                    ../../../lib/GNSSEph/BrcClockCorrection.cpp
                    ../../../lib/GNSSEph/BrcKeplerOrbit.cpp
                    ../../../lib/TimeHandling/CivilTime.cpp
                    ../../../lib/GNSSEph/ClockSatStore.cpp
                    ../../../lib/TimeHandling/CommonTime.cpp
                    ../../../lib/GNSSEph/EngAlmanac.cpp
                    ../../../lib/GNSSEph/EngEphemeris.cpp
                    ../../../lib/GNSSEph/EngNav.cpp
                    ../../../lib/Utilities/Exception.cpp
                    ../../../lib/Math/Expression.cpp
                    ../../../lib/ProcLib/ExtractData.cpp
                    ../../../lib/FileHandling/FFData.cpp
                    ../../../lib/FileHandling/FFStream.cpp
                    ../../../lib/FileHandling/FIC/FICData.cpp
                    ../../../lib/FileHandling/FIC/FICHeader.cpp
                    ../../../lib/GNSSEph/GalEphemeris.cpp
                    ../../../lib/GNSSEph/GalEphemerisStore.cpp
                    ../../../lib/GNSSEph/GloEphemeris.cpp
                    ../../../lib/GNSSEph/GloEphemerisStore.cpp
                    ../../../lib/GNSSEph/GPSAlmanacStore.cpp
                    ../../../lib/GNSSEph/GPSEphemeris.cpp
                    ../../../lib/GNSSEph/GPSEphemerisStore.cpp
                    ../../../lib/Utilities/gpstkplatform.h
                    ../../../lib/TimeHandling/GPSWeek.cpp
                    ../../../lib/TimeHandling/GPSWeekZcount.cpp
                    ../../../lib/TimeHandling/GPSZcount.cpp
                    ../../../lib/TimeHandling/JulianDate.cpp
                    ../../../lib/TimeHandling/MJD.cpp
                    ../../../lib/AstroEph/MoonPosition.cpp
                    ../../../lib/FileHandling/MSC/MSCData.cpp
                    ../../../lib/GNSSEph/MSCStore.cpp
                    ../../../lib/ProcLib/ObsEpochMap.cpp
                    ../../../lib/GNSSCore/ObsID.cpp
                    ../../../lib/GNSSCore/ObsIDInitializer.cpp
                    ../../../lib/GNSSEph/OrbElem.cpp
                    ../../../lib/GNSSEph/OrbElemRinex.cpp
                    ../../../lib/GNSSEph/OrbitEph.cpp
                    ../../../lib/GNSSEph/OrbitEphStore.cpp
                    ../../../lib/GNSSCore/PoleTides.cpp
                    ../../../lib/GNSSCore/Position.cpp
                    ../../../lib/GNSSEph/PositionSatStore.cpp
                    ../../../lib/PosSol/PRSolution2.cpp
                    ../../../lib/GNSSEph/QZSEphemeris.cpp
                    ../../../lib/GNSSEph/QZSEphemerisStore.cpp
                    ../../../lib/RefTime/ReferenceFrame.cpp
                    ../../../lib/FileHandling/RINEX3/Rinex3ClockData.cpp
                    ../../../lib/FileHandling/RINEX3/Rinex3ClockHeader.cpp
                    ../../../lib/GNSSEph/Rinex3EphemerisStore.cpp
                    ../../../lib/FileHandling/RINEX3/Rinex3NavData.cpp
                    ../../../lib/FileHandling/RINEX3/Rinex3NavHeader.cpp
                    ../../../lib/FileHandling/RINEX3/Rinex3ObsData.cpp
                    ../../../lib/FileHandling/RINEX3/Rinex3ObsHeader.cpp
                    ../../../lib/FileHandling/RINEX/RinexClockData.cpp
                    ../../../lib/FileHandling/RINEX/RinexClockHeader.cpp
                    ../../../lib/GNSSEph/RinexEphemerisStore.cpp
                    ../../../lib/FileHandling/RINEX/RinexMetData.cpp
                    ../../../lib/FileHandling/RINEX/RinexMetHeader.cpp
                    ../../../lib/FileHandling/RINEX/RinexNavData.cpp
                    ../../../lib/FileHandling/RINEX/RinexNavHeader.cpp
                    ../../../lib/FileHandling/RINEX/RinexObsData.cpp
                    ../../../lib/FileHandling/RINEX/RinexObsHeader.cpp
                    ../../../lib/GNSSEph/RinexObsID.cpp
                    ../../../lib/GNSSEph/RinexSatID.cpp
                    ../../../lib/FileHandling/RINEX/RinexUtilities.cpp
                    ../../../lib/GNSSEph/SEMAlmanacStore.cpp
                    ../../../lib/FileHandling/SEM/SEMData.cpp
                    ../../../lib/FileHandling/SEM/SEMHeader.cpp
                    ../../../lib/GNSSCore/SolidTides.cpp
                    ../../../lib/FileHandling/SP3/SP3Data.cpp
                    ../../../lib/GNSSEph/SP3EphemerisStore.cpp
                    ../../../lib/FileHandling/SP3/SP3Header.cpp
                    ../../../lib/GNSSEph/SP3SatID.cpp
                    ../../../lib/AstroEph/SunPosition.cpp
                    ../../../lib/Misc/SVNumXRef.cpp
                    ../../../lib/TimeHandling/SystemTime.cpp
                    ../../../lib/TimeHandling/TimeConverters.cpp
                    ../../../lib/TimeHandling/TimeString.cpp
                    ../../../lib/RefTime/TimeSystem.cpp
                    ../../../lib/TimeHandling/TimeTag.cpp
                    ../../../lib/Math/Triple.cpp
                    ../../../lib/GNSSCore/TropModel.cpp
                    ../../../lib/TimeHandling/UnixTime.cpp
                    ../../../lib/TimeHandling/Week.cpp
                    ../../../lib/TimeHandling/WeekSecond.cpp
                    ../../../lib/GNSSCore/WxObsMap.cpp
                    ../../../lib/GNSSCore/Xvt.cpp
                    ../../../lib/TimeHandling/YDSTime.cpp
                    ../../../lib/GNSSEph/YumaAlmanacStore.cpp
                    ../../../lib/FileHandling/Yuma/YumaData.cpp
                    )
