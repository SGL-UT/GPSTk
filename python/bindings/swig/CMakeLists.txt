# This file is not added to the parent cmakelists.txt files!

cmake_minimum_required(VERSION 2.8)

#---------------------------------------------------------------------------------
# Installation Path Prefix - Do not change unless required to desired location
#---------------------------------------------------------------------------------

set (CMAKE_INSTALL_PREFIX $ENV{gpstk})
set (CMAKE_CPP_LIB_PREFIX "${CMAKE_INSTALL_PREFIX}/../dev/ext/lib")

#---------------------------------------------------------------------------------

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
message(STATUS "Building using swig at: ${SWIG_EXECUTABLE}")

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(
                    ${CMAKE_CPP_LIB_PREFIX}/AstroEph
                    ${CMAKE_CPP_LIB_PREFIX}/ClockModel
                    ${CMAKE_CPP_LIB_PREFIX}/deprecate
                    ${CMAKE_CPP_LIB_PREFIX}/FileDirProc
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX3
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/SEM
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/SP3
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/Yuma
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSCore
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph
                    ${CMAKE_CPP_LIB_PREFIX}/Math
                    ${CMAKE_CPP_LIB_PREFIX}/Math/Matrix
                    ${CMAKE_CPP_LIB_PREFIX}/Math/Vector
                    ${CMAKE_CPP_LIB_PREFIX}/Misc
                    ${CMAKE_CPP_LIB_PREFIX}/PosSol
                    ${CMAKE_CPP_LIB_PREFIX}/Procframe
                    ${CMAKE_CPP_LIB_PREFIX}/RefTime
                    ${CMAKE_CPP_LIB_PREFIX}/Rxio
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling
                    ${CMAKE_CPP_LIB_PREFIX}/Utilities
                    )

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
#set_source_files_properties(gpstk_pylib.i PROPERTIES SWIG_FLAGS "-includeall") //Possible addition for full source file library
swig_add_module(gpstk_pylib python gpstk_pylib.i
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/AlmOrbit.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/ANSITime.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/Procframe/Antenna.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/AstroEph/AstronomicalFunctions.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/BDSEphemeris.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/BDSEphemerisStore.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/Utilities/BinUtils.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/BrcClockCorrection.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/BrcKeplerOrbit.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/CivilTime.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/ClockSatStore.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/CommonTime.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/EngAlmanac.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/EngEphemeris.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/EngNav.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/Utilities/Exception.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/Math/Expression.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/Procframe/ExtractData.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/FFData.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/FFStream.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/GalEphemeris.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/GalEphemerisStore.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/GloEphemeris.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/GloEphemerisStore.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/GPSAlmanacStore.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/GPSEphemeris.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/GPSEphemerisStore.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/Utilities/gpstkplatform.h
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/GPSWeek.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/GPSWeekZcount.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/GPSZcount.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/JulianDate.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/MJD.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/AstroEph/MoonPosition.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/ClockModel/ObsEpochMap.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSCore/ObsID.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSCore/ObsIDInitializer.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/OrbElem.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/OrbElemRinex.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/OrbitEph.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/OrbitEphStore.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSCore/PoleTides.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSCore/Position.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/PositionSatStore.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/PosSol/PRSolution2.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/QZSEphemeris.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/QZSEphemerisStore.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/RefTime/ReferenceFrame.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX3/Rinex3ClockData.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX3/Rinex3ClockHeader.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/Rinex3EphemerisStore.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX3/Rinex3NavData.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX3/Rinex3NavHeader.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX3/Rinex3ObsData.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX3/Rinex3ObsHeader.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX/RinexClockData.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX/RinexClockHeader.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/RinexEphemerisStore.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX/RinexMetData.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX/RinexMetHeader.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX/RinexNavData.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX/RinexNavHeader.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX/RinexObsData.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX/RinexObsHeader.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/RinexObsID.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/RinexSatID.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/RINEX/RinexUtilities.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/SEMAlmanacStore.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/SEM/SEMData.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/SEM/SEMHeader.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSCore/SolidTides.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/SP3/SP3Data.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/SP3EphemerisStore.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/SP3/SP3Header.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/SP3SatID.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/AstroEph/SunPosition.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/Misc/SVNumXRef.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/SystemTime.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/TimeConverters.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/TimeString.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/RefTime/TimeSystem.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/TimeTag.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/Math/Triple.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSCore/TropModel.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/UnixTime.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/Week.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/WeekSecond.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSCore/WxObsMap.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSCore/Xvt.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/TimeHandling/YDSTime.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/YumaAlmanacStore.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/Yuma/YumaData.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/GNSSEph/EphemerisRange.cpp
#                   ${CMAKE_CPP_LIB_PREFIX}/Rxio/EphReader.cpp
#                   ${CMAKE_CPP_LIB_PREFIX}/Rxio/FFIdentifier.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/Rxio/NovatelData.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/Rxio/AshtechData.cpp
                    ${CMAKE_CPP_LIB_PREFIX}/FileHandling/SMODFData.cpp
                    )

execute_process(COMMAND python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()" OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)