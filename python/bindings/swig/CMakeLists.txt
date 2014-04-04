# this file is not added to parent cmakelists.txt files
cmake_minimum_required(VERSION 2.8)

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
message(STATUS "Building using swig at: ${SWIG_EXECUTABLE}")

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../../../dev/ext/lib/AstroEph
                    ../../../dev/ext/lib/ClockModel
                    ../../../dev/ext/lib/deprecate
                    ../../../dev/ext/lib/FileDirProc
                    ../../../dev/ext/lib/FileHandling
                    ../../../dev/ext/lib/FileHandling/RINEX
                    ../../../dev/ext/lib/FileHandling/RINEX3
                    ../../../dev/ext/lib/FileHandling/SEM
                    ../../../dev/ext/lib/FileHandling/SP3
                    ../../../dev/ext/lib/FileHandling/Yuma
                    ../../../dev/ext/lib/GNSSCore
                    ../../../dev/ext/lib/GNSSEph
                    ../../../dev/ext/lib/Math
                    ../../../dev/ext/lib/Math/Matrix
                    ../../../dev/ext/lib/Math/Vector
                    ../../../dev/ext/lib/Misc
                    ../../../dev/ext/lib/PosSol
                    ../../../dev/ext/lib/Procframe
                    ../../../dev/ext/lib/RefTime
     		     ../../../dev/ext/lib/Rxio
                    ../../../dev/ext/lib/TimeHandling
                    ../../../dev/ext/lib/Utilities )

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
                    ../../../dev/ext/lib/GNSSEph/AlmOrbit.cpp
                    ../../../dev/ext/lib/TimeHandling/ANSITime.cpp
                    ../../../dev/ext/lib/Procframe/Antenna.cpp
                    ../../../dev/ext/lib/AstroEph/AstronomicalFunctions.cpp
                    ../../../dev/ext/lib/GNSSEph/BDSEphemeris.cpp
                    ../../../dev/ext/lib/GNSSEph/BDSEphemerisStore.cpp
                    ../../../dev/ext/lib/Utilities/BinUtils.cpp
                    ../../../dev/ext/lib/GNSSEph/BrcClockCorrection.cpp
                    ../../../dev/ext/lib/GNSSEph/BrcKeplerOrbit.cpp
                    ../../../dev/ext/lib/TimeHandling/CivilTime.cpp
                    ../../../dev/ext/lib/GNSSEph/ClockSatStore.cpp
                    ../../../dev/ext/lib/TimeHandling/CommonTime.cpp
                    ../../../dev/ext/lib/GNSSEph/EngAlmanac.cpp
                    ../../../dev/ext/lib/GNSSEph/EngEphemeris.cpp
                    ../../../dev/ext/lib/GNSSEph/EngNav.cpp
                    ../../../dev/ext/lib/Utilities/Exception.cpp
                    ../../../dev/ext/lib/Math/Expression.cpp
                    ../../../dev/ext/lib/Procframe/ExtractData.cpp
                    ../../../dev/ext/lib/FileHandling/FFData.cpp
                    ../../../dev/ext/lib/FileHandling/FFStream.cpp
                    ../../../dev/ext/lib/GNSSEph/GalEphemeris.cpp
                    ../../../dev/ext/lib/GNSSEph/GalEphemerisStore.cpp
                    ../../../dev/ext/lib/GNSSEph/GloEphemeris.cpp
                    ../../../dev/ext/lib/GNSSEph/GloEphemerisStore.cpp
                    ../../../dev/ext/lib/GNSSEph/GPSAlmanacStore.cpp
                    ../../../dev/ext/lib/GNSSEph/GPSEphemeris.cpp
                    ../../../dev/ext/lib/GNSSEph/GPSEphemerisStore.cpp
                    ../../../dev/ext/lib/Utilities/gpstkplatform.h
                    ../../../dev/ext/lib/TimeHandling/GPSWeek.cpp
                    ../../../dev/ext/lib/TimeHandling/GPSWeekZcount.cpp
                    ../../../dev/ext/lib/TimeHandling/GPSZcount.cpp
                    ../../../dev/ext/lib/TimeHandling/JulianDate.cpp
                    ../../../dev/ext/lib/TimeHandling/MJD.cpp
                    ../../../dev/ext/lib/AstroEph/MoonPosition.cpp
                    ../../../dev/ext/lib/ClockModel/ObsEpochMap.cpp
                    ../../../dev/ext/lib/GNSSCore/ObsID.cpp
                    ../../../dev/ext/lib/GNSSCore/ObsIDInitializer.cpp
                    ../../../dev/ext/lib/GNSSEph/OrbElem.cpp
                    ../../../dev/ext/lib/GNSSEph/OrbElemRinex.cpp
                    ../../../dev/ext/lib/GNSSEph/OrbitEph.cpp
                    ../../../dev/ext/lib/GNSSEph/OrbitEphStore.cpp
                    ../../../dev/ext/lib/GNSSCore/PoleTides.cpp
                    ../../../dev/ext/lib/GNSSCore/Position.cpp
                    ../../../dev/ext/lib/GNSSEph/PositionSatStore.cpp
                    ../../../dev/ext/lib/PosSol/PRSolution2.cpp
                    ../../../dev/ext/lib/GNSSEph/QZSEphemeris.cpp
                    ../../../dev/ext/lib/GNSSEph/QZSEphemerisStore.cpp
                    ../../../dev/ext/lib/RefTime/ReferenceFrame.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ClockData.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ClockHeader.cpp
                    ../../../dev/ext/lib/GNSSEph/Rinex3EphemerisStore.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX3/Rinex3NavData.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX3/Rinex3NavHeader.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ObsData.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ObsHeader.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX/RinexClockData.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX/RinexClockHeader.cpp
                    ../../../dev/ext/lib/GNSSEph/RinexEphemerisStore.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX/RinexMetData.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX/RinexMetHeader.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX/RinexNavData.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX/RinexNavHeader.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX/RinexObsData.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX/RinexObsHeader.cpp
                    ../../../dev/ext/lib/GNSSEph/RinexObsID.cpp
                    ../../../dev/ext/lib/GNSSEph/RinexSatID.cpp
                    ../../../dev/ext/lib/FileHandling/RINEX/RinexUtilities.cpp
                    ../../../dev/ext/lib/GNSSEph/SEMAlmanacStore.cpp
                    ../../../dev/ext/lib/FileHandling/SEM/SEMData.cpp
                    ../../../dev/ext/lib/FileHandling/SEM/SEMHeader.cpp
                    ../../../dev/ext/lib/GNSSCore/SolidTides.cpp
                    ../../../dev/ext/lib/FileHandling/SP3/SP3Data.cpp
                    ../../../dev/ext/lib/GNSSEph/SP3EphemerisStore.cpp
                    ../../../dev/ext/lib/FileHandling/SP3/SP3Header.cpp
                    ../../../dev/ext/lib/GNSSEph/SP3SatID.cpp
                    ../../../dev/ext/lib/AstroEph/SunPosition.cpp
                    ../../../dev/ext/lib/Misc/SVNumXRef.cpp
                    ../../../dev/ext/lib/TimeHandling/SystemTime.cpp
                    ../../../dev/ext/lib/TimeHandling/TimeConverters.cpp
                    ../../../dev/ext/lib/TimeHandling/TimeString.cpp
                    ../../../dev/ext/lib/RefTime/TimeSystem.cpp
                    ../../../dev/ext/lib/TimeHandling/TimeTag.cpp
                    ../../../dev/ext/lib/Math/Triple.cpp
                    ../../../dev/ext/lib/GNSSCore/TropModel.cpp
                    ../../../dev/ext/lib/TimeHandling/UnixTime.cpp
                    ../../../dev/ext/lib/TimeHandling/Week.cpp
                    ../../../dev/ext/lib/TimeHandling/WeekSecond.cpp
                    ../../../dev/ext/lib/GNSSCore/WxObsMap.cpp
                    ../../../dev/ext/lib/GNSSCore/Xvt.cpp
                    ../../../dev/ext/lib/TimeHandling/YDSTime.cpp
                    ../../../dev/ext/lib/GNSSEph/YumaAlmanacStore.cpp
                    ../../../dev/ext/lib/FileHandling/Yuma/YumaData.cpp
		    ../../../dev/ext/lib/GNSSEph/EphemerisRange.cpp
#		    ../../../dev/ext/lib/Rxio/EphReader.cpp
#		    ../../../dev/ext/lib/Rxio/FFIdentifier.cpp
		    ../../../dev/ext/lib/Rxio/NovatelData.cpp
		    ../../../dev/ext/lib/Rxio/AshtechData.cpp
		    ../../../dev/ext/lib/FileHandling/SMODFData.cpp
                    )
