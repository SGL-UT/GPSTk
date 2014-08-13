cmake_minimum_required(VERSION 2.8)

# Please note that this file does not use any of the CMakeLists files in the dev
# directory. This is essentially a separate project that uses the source code in the
# ../../../dev directory.
project (gpstk_bindings)

# The default CMAKE_INSTALL_PREFIX is inappropriate for python modules so this variable needs
# to be overridden. To install in the user's directory:
#   cmake -DCMAKE_INSTALL_PREFIX=$(python -m site --user-site) ..


# Since the default is never correct, at least put in something reasonable
if ("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local")
  execute_process(COMMAND python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()"
    OUTPUT_VARIABLE CMAKE_INSTALL_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

# This is a hack way to find the GPSTk source directory. This assumes the current
# directory part of a complete clone of the GPSTk repo.
set (GPSTK_SRC "../../../dev")
set (GPSTK_EXT_LIB "${GPSTK_SRC}/ext/lib")
set (GPSTK_CORE_LIB "${GPSTK_SRC}/core/lib")

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
message(STATUS "Building using swig at: ${SWIG_EXECUTABLE}")
message(STATUS "Installing to: ${CMAKE_INSTALL_PREFIX}")

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(
  ${GPSTK_EXT_LIB}/AstroEph
  ${GPSTK_EXT_LIB}/ClockModel
  ${GPSTK_EXT_LIB}/deprecate
  ${GPSTK_EXT_LIB}/FileDirProc
  ${GPSTK_EXT_LIB}/FileHandling
  ${GPSTK_EXT_LIB}/FileHandling/RINEX
  ${GPSTK_EXT_LIB}/FileHandling/RINEX3
  ${GPSTK_EXT_LIB}/FileHandling/SEM
  ${GPSTK_EXT_LIB}/FileHandling/SP3
  ${GPSTK_EXT_LIB}/FileHandling/Yuma
  ${GPSTK_EXT_LIB}/GNSSCore
  ${GPSTK_EXT_LIB}/GNSSEph
  ${GPSTK_EXT_LIB}/Math
  ${GPSTK_EXT_LIB}/Math/Matrix
  ${GPSTK_EXT_LIB}/Math/Vector
  ${GPSTK_EXT_LIB}/Misc
  ${GPSTK_EXT_LIB}/PosSol
  ${GPSTK_EXT_LIB}/Procframe
  ${GPSTK_EXT_LIB}/RefTime
  ${GPSTK_EXT_LIB}/Rxio
  ${GPSTK_EXT_LIB}/TimeHandling
  ${GPSTK_EXT_LIB}/Utilities
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
swig_add_module(
  gpstk_pylib
  python
  gpstk_pylib.i
  ${GPSTK_EXT_LIB}/GNSSEph/AlmOrbit.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/ANSITime.cpp
  ${GPSTK_EXT_LIB}/Procframe/Antenna.cpp
  ${GPSTK_EXT_LIB}/AstroEph/AstronomicalFunctions.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/BDSEphemeris.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/BDSEphemerisStore.cpp
  ${GPSTK_EXT_LIB}/Utilities/BinUtils.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/BrcClockCorrection.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/BrcKeplerOrbit.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/CivilTime.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/ClockSatStore.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/CommonTime.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/EngAlmanac.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/EngEphemeris.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/EngNav.cpp
  ${GPSTK_EXT_LIB}/Utilities/Exception.cpp
  ${GPSTK_EXT_LIB}/Math/Expression.cpp
  ${GPSTK_EXT_LIB}/Procframe/ExtractData.cpp
  ${GPSTK_EXT_LIB}/FileHandling/FFData.cpp
  ${GPSTK_EXT_LIB}/FileHandling/FFStream.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/GalEphemeris.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/GalEphemerisStore.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/GloEphemeris.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/GloEphemerisStore.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/GPSAlmanacStore.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/GPSEphemeris.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/GPSEphemerisStore.cpp
  ${GPSTK_EXT_LIB}/Utilities/gpstkplatform.h
  ${GPSTK_EXT_LIB}/TimeHandling/GPSWeek.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/GPSWeekZcount.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/GPSZcount.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/JulianDate.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/MJD.cpp
  ${GPSTK_EXT_LIB}/AstroEph/MoonPosition.cpp
  ${GPSTK_EXT_LIB}/ClockModel/ObsEpochMap.cpp
  ${GPSTK_EXT_LIB}/GNSSCore/ObsID.cpp
  ${GPSTK_EXT_LIB}/GNSSCore/ObsIDInitializer.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/OrbElem.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/OrbElemRinex.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/OrbitEph.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/OrbitEphStore.cpp
  ${GPSTK_EXT_LIB}/GNSSCore/PoleTides.cpp
  ${GPSTK_EXT_LIB}/GNSSCore/Position.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/PositionSatStore.cpp
  ${GPSTK_EXT_LIB}/PosSol/PRSolution2.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/QZSEphemeris.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/QZSEphemerisStore.cpp
  ${GPSTK_EXT_LIB}/RefTime/ReferenceFrame.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX3/Rinex3ClockData.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX3/Rinex3ClockHeader.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/Rinex3EphemerisStore.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX3/Rinex3NavData.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX3/Rinex3NavHeader.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX3/Rinex3ObsData.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX3/Rinex3ObsHeader.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX/RinexClockData.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX/RinexClockHeader.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/RinexEphemerisStore.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX/RinexMetData.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX/RinexMetHeader.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX/RinexNavData.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX/RinexNavHeader.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX/RinexObsData.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX/RinexObsHeader.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/RinexObsID.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/RinexSatID.cpp
  ${GPSTK_EXT_LIB}/FileHandling/RINEX/RinexUtilities.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/SEMAlmanacStore.cpp
  ${GPSTK_EXT_LIB}/FileHandling/SEM/SEMData.cpp
  ${GPSTK_EXT_LIB}/FileHandling/SEM/SEMHeader.cpp
  ${GPSTK_EXT_LIB}/GNSSCore/SolidTides.cpp
  ${GPSTK_EXT_LIB}/FileHandling/SP3/SP3Data.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/SP3EphemerisStore.cpp
  ${GPSTK_EXT_LIB}/FileHandling/SP3/SP3Header.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/SP3SatID.cpp
  ${GPSTK_EXT_LIB}/AstroEph/SunPosition.cpp
  ${GPSTK_EXT_LIB}/Misc/SVNumXRef.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/SystemTime.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/TimeConverters.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/TimeString.cpp
  ${GPSTK_EXT_LIB}/RefTime/TimeSystem.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/TimeTag.cpp
  ${GPSTK_EXT_LIB}/Math/Triple.cpp
  ${GPSTK_EXT_LIB}/GNSSCore/TropModel.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/UnixTime.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/Week.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/WeekSecond.cpp
  ${GPSTK_EXT_LIB}/GNSSCore/WxObsMap.cpp
  ${GPSTK_EXT_LIB}/GNSSCore/Xvt.cpp
  ${GPSTK_EXT_LIB}/TimeHandling/YDSTime.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/YumaAlmanacStore.cpp
  ${GPSTK_EXT_LIB}/FileHandling/Yuma/YumaData.cpp
  ${GPSTK_EXT_LIB}/GNSSEph/EphemerisRange.cpp
  # ${GPSTK_EXT_LIB}/Rxio/EphReader.cpp
  # ${GPSTK_EXT_LIB}/Rxio/FFIdentifier.cpp
  ${GPSTK_EXT_LIB}/Rxio/NovatelData.cpp
  ${GPSTK_EXT_LIB}/Rxio/AshtechData.cpp
  ${GPSTK_EXT_LIB}/FileHandling/SMODFData.cpp
  )

install(CODE "execute_process(COMMAND ../gpstk_builder.py ${CMAKE_INSTALL_PREFIX})")
