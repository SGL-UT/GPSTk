GPSTk 2.10 Release Notes
========================

Updates since v2.9
------------------

**Build System and Test Suite**
 * Significant cleanup of Windows Test Suite
 * End-of-line Normalization implemented.
 * Updated build script for supporting VS 14 2015 install via command line.
 * Native Debian package support has been added, using native Debian packaging tools rather than CPack-based support and is more compliant with Debian policy than the CPack version. Debian package is split into multiple packages - gpstk-bin contains the binaries; libgpstkx.x contains the shared library objects; libgpstk-dev contains the headers for development; python-gpstk contains the Python bindings (and swig headers). Supports side-by-side installation of multiple versions of libgpstk.so, so as to facilitate staged upgrades and backward compatibility. Dependencies are computed (and done using native Debian tools), and so will trigger upgrades/conflicts appropriately. Packages are far more Lintian clean (big remaining item is missing man-pages). Work towards supporting more standards-compliant system-wide installations, independent of installation methods (e.g. RPM, Debian, ./build.sh -s).


**Gitlab CI**
 * Core build stage added to detect reverse dependency between core and ext
 * Solaris Runner deprecated, only built as nightly
 * Redhat Runner added to Gitlab CI platforms
 * Major Overhaul of Gitlab CI infrastructure.  Originally consisting of only Build and Test stages, the new design takes advantage of artifact passing for better efficiency and faster builds while adding new stages - Deploy (local install), Package, and Downstream (SGLTk) stages. System install, license check, and additional stages are planned in the future.    


**Documentation**
 * Documentation across the GPSTk has been revamped. User's Reference Manual has been updated - old application removed, migrated applications moved to SGLTk manual. Documentation.md added for how-tos on building documentation. Application Documentation added to each app directory, in markdown for easy reading and auto-rendering on Gitlab.


**Library Changes**
 * **FUTURE DEPRECATION WARNING** for CommonTime public accessor/mutator methods.  REASON:  Discussions for Issue #248 have led to us retaining the internal GPSTk representation of Julian Date, jday, and instead restrict CommonTime use to specific classes as designed.
 * Documentation describing the internal GPSTk representation of Julian Date, jday(JD+0.5), was taken from DayTime (deprecated) and added to JulianDate.hpp. REASON: JulianDate.hpp lacked documentation for internal GPSTk representation of Julian Date, jday, which used to exist before the DayTime to CommonTime/TimeTag update.
 * EngEphemeris objects now initialize the AODO (Age of Data Offset) upon creation. REASON: The maximum age that the satellites can broadcast is 27900 seconds (5-bit unsigned multiplied by 900. 31*900=27900). After 27900 seconds the offset data is unusable. Since a fresh EngEphemeris object contains ephemeris of an unknown age, it makes sense that AODO is set to 27900 to indicate that the offset data should not be relied upon.
 * GPS CNAV added to Navfilter, which entails adding a variety of CNavFilter classes, adding debug print capability to NavFilter and descendants, as well as a variety of additions to the functionality of core/GNSSEph/PackedNavBits. REASON: In order to develop GPS CNAV (L2 and L5) processing for MDH and MDP into RawNavCSV files, we need the capability to compare CNAV messages across receivers (at least).
 * Trop Models refactored to have one model per file by refactoring exception throwing.  REASON: Preservation of location information.  
 * Extension of OrbElemBase Hierarchy to Support "Almanac" Data.  New LNAV subclasses for virtual OrbSysGPSL, which provides a means to identify contents of the objects by either subframe/page or by Data ID - OrbSysGPL_51 (SV Health for PRN 1-24), OrbSysGPL_52 (Navigation Message Correction Table), OrbSysGPL_53 (Special message), OrbSysGPL_56 (Iono model, UTC correction, Leap second), OrbSysGPL_63 (SV Configuration (all SV) and SV Health for PRN 25-32). CNAV_L2 and CNAV_L5 string IDs changed. REASON: Broaden OrbElemBase coverage to encompass both lower-precision "almanac" orbits and the non-orbit "system-level" data.
 * NavID was added to PackedNavBits. REASON: BeiDou ICD was discovered to be incorrect when it states: 1) PRN 1-5 will always be GEO and always broadcast nav ms format D2 2) PRN >=6 will always be MEO/IGSO and always broadcast nav msg format D1. This means the software cannot depend on SatID alone (PRN and system) to determine D1 or D2 format.  As the MDH format will eventually include a NAV code, a short-term solution is to examine the nav message data rate to determine D1 or D2 and then initialize the NavID associated with the PackedNavBits (as it stores the bits and other associated meta-data) to keep track of that determination.
 * Increased the maximum number of iterations for invChisqCDF, contfracIncompGamma, cfIBeta, and seriesIncompGamma from 100 to 1000. REASON: In LSA, we encountered a problem with a large number of degrees of freedom (26301) and invChisqCDF failed to converge after 100 iterations. Brian Tolman suggested that simply increasing this limit should be sufficient. I bumped up niter from 100 to 1000 for invChisqCDF, and also invNormalCDF, invStudentsCDF for consistency. This is an arbitrary value and I have not determined the number of iterations required to converge for the problem encountered in LSA.
 * Updated TropModel to treat humidity values between 100 and 105 as 100 and to throw an exception if above 105. REASON: A Value of more than 100 for humidity will throw an exception in TropModel. However, this does not play well with real humidity values which can be above 100. Two solutions exist: the max humidity can be raised in TropModel or rather than throwing an exception if humidity is above 100, TropModel can treat high humidity as 100 percent.
 * OrbAlmGlo modified to use beginValid instead of the Almanac Week number (WNa) Time of Arrival (toa).  Originally, it was assumed that the almanac WNa/toa was "in the future" from the curren time by at least a day.  Based on observed behavior, it seems that the almanac WNA/toa is actually within a half-day of the current time, although no promises are made on this matter.
 * Operator <() added to TimeRange to allow use of TimeRange as key in set/map.  Operator test added as well.
 * OrbitEphStore find methods return NULL when OrbitEphStore is empty.  REASON: Normal circumstances dictate the map for a given satellite would not be emptied, but this can't be guaranteed as soon as edit() is used. 
 * Fix usage of Position in ORD classes. Modified ClockModel/ObsRngDev.cpp and GNSSEph/EphemerisRange.cpp to use Position with conversions to the appropriate CoordinateSystem. Fixes exceptions when a CoordinateSystem other than the one expected/assumed was passed in. REASON: The ObsRngDev methods are trying to create Geodetic positions from ECEF input.
 * Fixed BINEX CRC length and added unit tests to catch incorrect CRC length and other record length problems; updateded unit tests with fancy macros. REASON: BINEX CRC length was incorrect.
 * Infrastructure changes made for additional BeiDou support.
 * A method was added to OrbSysGpsL_56 to create a TimeSystemCorrelation Object. Also added a new .find( ) method to OrbSysStore that returns the most recently transmitted version of a particular message type across all SVs, which is needed to find most recent UTC Offset data across the constellation. REASON: Given a OrbSysGPSL_56 exists, it should be able to create a TimeSystemCorrection object based on the A0/A1 and related terms.   The resulting TimeSystemCorrection object should work exactly as if it was read from a RINEX header "GPUT" record.
 * BDS and QZSS support added to SP3c parsins. REASON: SP3c support is incomplete and fails when confronted with GNSS system codes for BeiDou and QZSS. This issue aims to patch this hole and add tests to confirm good operation of the new code.
 * Antenna phase center offset calculations added.
 * Infrastructure changes made to support analysis of IRNSS data, for example, updating OrbAlmGen.cpp to include capability for creating IRNSS orbit comparisons. REASON: Various infrastructure pieces need to be updated to support analysis of data from the Indian Regional Navigation Satellite System (IRNSS).  
 * Stream flags are changed and restored as needed in hexDumpData. Additional improvements to hexDumpData as well.  REASON: hexDumpData does a bit of stream manipulation and also makes assumptions about the state of the stream to start with.  One possible end result is that you may have left justification turned on, in which case index numbers like 0x10 will be printed as "1000", the final 2 "0" characters being the stream fill character.  Byte data that is < 16, e.g. 0-f could be printed as, say "c0" instead of "0c".
 * RINEX 3 Nav Xmit time output is now actually Xmit time (instead of HOWTime). Fixed Rinex3NavData.toList() including Toc twice and Toe 0 times Rinex3NavHeader output better matches RINEX specifications. 
 * RINEX 2 header date formatted to match previous RINEX class output.
 * RINEX 3.03 (July 2014) support is now complete, which involved completing IRNSS support and updating Beidou frequency B1 to appear as '2' as in Obs ID (as with RINEX 3.00, 3.01) rather than '1' as it does in RINEX 3.02. 
 * Small update to CommandLine - No args means help as default; verbose, debug, and help added.
 * Added message handler for CNAV MT 32, as well as tests for MT32 and MT33. REASON: While working on RFC-354 for the public signal ICWG I run across the need to implement an OrbDataSys class to handle GPS CNAV MT 32.   I need to both implement the cracker and add it to the ext/GNSSEph/OrbDataSysFactory class.
 * Rationalize function was moved from GPSOrbElemStore to OrbElemStore.
 * Updated SVNumXRef to have the correct dates and times as verified through rawNavDump, as well as for the change from sv49/prn4 to sv36/prn4 from the 2017070 nanu.
 * Solar System update - Extensive updates to Earth orientation code, including adding the JPL 405 ephemeris, IERS conventions for 1996, 2003 and 2010, and tests compare results with IAU, IERS and JPL test code results.
 * Added frequency and wavelength constants for Glonass G3 carrier. REASON: The Glonass G3 wavelength and frequency were not in the GNSSConstants.hpp file. 
 * Global Trop model has been added along with minor additions to TropModel classes. Tests also added for each of the trop models that dumps the values and compares to the expected outputs.
 * Updated SatPass to write RINEX 3 files with SatPassToRinex3File()). Also replaced SatPass::sort() with std::sort(), deprecated SatPass::SatPassToRinexFile() and replaced it with SatPassToRinex2File().
 * Improve Stats by defining wtd and sequential as separate classes, add simple statistical filter class StatsFilter to geomatics. Added new tests of Stats as well as tests of StatsFilter.
 * In Rinex3NavData, replaced HOWtime with xmitTime because the RINEX spec asks for xmitTime and not HOWtime. Added fromLongDouble() to MJD class. Added const qualifier to Stats::operator+= and -=. Updated RINEX Nav truth data. REASON: MSN Requirement.
 * Rinex3ObsHeader output date set as yyyymmdd hhmmss zone regardless of RINEX version.  REASON: Rinex3ObsHeader output a less specific Date for RINEX 2 files than RINEX 3 and previous RINEX output.
 * Added a wrapper for POSIX struct timespec, TimeHandling/PosixTime REASON: The current UnixTime class is designed around struct timeval, used by the gettimeofday() function. The (RHEL7) Linux man page for gettimeofday says: "POSIX.1-2008 marks gettimeofday() as obsolete, recommending the use of clock_gettime(2) instead." The clock_gettime function uses struct timespec instead, which uses nanoseconds instead of microseconds, so a new class implementation is necessary to support it.
 * In TimeString, TimeTag identifier for integer full QZS Week changed from 'I' to 'h' as part of PosixTime implementation.
 * CNAV filters have been modified to no longer discard default message data, as originally implemented.  REASON: Downstream processes need to know about default navigation message periods.  Therefore, we can't simply discard the data.

*New Library Classes*
 * core/lib/GNSSCore/CSCG2000Ellipsoid.hpp
 * core/lib/GNSSCore/GCATTropModel.*
 * core/lib/GNSSCore/GGHeightTropModel.*
 * core/lib/GNSSCore/GGTropModel.*
 * core/lib/GNSSCore/GlobalTropModel.*
 * core/lib/GNSSCore/MOPSTropModel.*
 * core/lib/GNSSCore/NBTropModel.*
 * core/lib/GNSSCore/NeillTropModel.*
 * core/lib/GNSSCore/SaasTropModel.*
 * core/lib/GNSSCore/SimpleTropModel.*
 * core/lib/GNSSEph/RationalizeRinexNav.*
 * core/lib/NavFilter/CNavCookFilter.*
 * core/lib/NavFilter/CNavCrossSourceFilter.*
 * core/lib/NavFilter/CNavEmptyFilter.*
 * core/lib/NavFilter/CNavFilterData.*
 * core/lib/NavFilter/CNavParityFilter.*
 * core/lib/NavFilter/CNavTOWFilter.*
 * core/lib/NavFilter/LNavFilterData.*
 * core/lib/TimeHandling/IRNWeekSecond.hpp
 * core/lib/Utilities/HexDumpDataConfig.*
 * ext/lib/Geomatics/AntennaStore.*
 * ext/lib/Geomatics/CubicSpline.hpp
 * ext/lib/Geomatics/EOPPrediction.*
 * ext/lib/Geomatics/EOPStore.*
 * ext/lib/Geomatics/EphTime.hpp
 * ext/lib/Geomatics/IERS1996NutationData.hpp
 * ext/lib/Geomatics/IERS1996UT1mUTCData.hpp
 * ext/lib/Geomatics/IERS2003NutationData.hpp
 * ext/lib/Geomatics/IERS2010CIOSeriesData.hpp
 * ext/lib/Geomatics/IERSConvention.*
 * ext/lib/Geomatics/OceanLoadTides.*
 * ext/lib/Geomatics/SolarSystemEphemeris.*
 * ext/lib/Geomatics/StatsFilter.hpp
 * ext/lib/GNSSEph/OrbAlam.*
 * ext/lib/GNSSEph/OrbAlmFactory.*
 * ext/lib/GNSSEph/OrbAlmGen.*
 * ext/lib/GNSSEph/OrbDataSysFactory.*
 * ext/lib/GNSSEph/OrbSysGpsC.*
 * ext/lib/GNSSEph/OrbSysGpsC_30.*
 * ext/lib/GNSSEph/OrbSysGpsC_32.*
 * ext/lib/GNSSEph/OrbSysGpsC_33.*
 * ext/lib/GNSSEph/OrbSysGpsL_52.*
 * ext/lib/GNSSEph/OrbSysGpsL_55.*
 * ext/lib/GNSSEph/OrbSysGpsL_Reserved.* 

*Moved/Renamed Library Classes*
 * ext/lib/Geomatics/DDid.* -> ext/apps/geomatics/relposition/DDid.*
 * ext/lib/Geomatics/index.hpp -> ext/apps/geomatics/relposition/index.hpp
 * ext/lib/Geomatics/PhaseWindup.* -> ext/apps/geomatics/relposition/PhaseWindup.*
 * ext/lib/GNSSEph/OrbElem.* -> core/lib/GNSSEph/OrbElem.*
 * ext/lib/GNSSEph/OrbElem.* -> core/lib/GNSSEph/OrbElem.*
 * ext/lib/GNSSEph/OrbElemBase.* -> core/lib/GNSSEph/OrbElemBase.*
 * ext/lib/GNSSEph/OrbElemRinex.* -> core/lib/GNSSEph/OrbElemRinex.*
 * ext/lib/GNSSEph/OrbElemStore.* -> core/lib/GNSSEph/OrbElemStore.*

*Deprecated/Deleted Library Classes*
 * ext/lib/Geomatics/GeodeticFrames.*
 * ext/lib/Geomatics/MostCommonValue.hpp


**Application Changes**
 * Calgps has been split into two applications - /core application of the same name with no /ext/lib/vdraw support and no support for postscript, eps, or svg - /ext application named calgps_svg with full /ext/lib/vdraw support and supports postscript, eps, and svg. Primary application tests have been migrated to /core/tests, while postscript,eps, and svg tests have been left in /ext/tests.
 * RINEX and RinEdit changes - Rinex3Met files can now be version 3.02. RinexObsHeaders without Glonass observations no longer require glonassCodPhsBias or glonassFreqNo. Added checking for invalid system chars in prepareVer2Write. Added checking for prepareVer2Write when attempting to output RINEX v2.11 files. Using prepareVer2Write to select between overlapping ObsID -> R2 Obs Type conversions now creates a header comment clarifying the origin of the Obs Type. Added tests for these changes, including test for creating RINEX Obs files from scratch. 
 * rnwdiff and rowdiff can now compare RINEX 3 files in addition to RINEX 2 files and can diff R2 and R3 files against each other. 
 * rowdiff - added missing newline character after epochs for readability; check that secondObsItr exists before attempting to compare against it.
 * New application - timediff |  A tool for getting the difference between two times
 * New application - scanBrdcFile | Scan an IGS-generated BRDC file in RINEX Nav format, fix various flaws, and write the results to a new RINEX nav file.  Optionally provide a summary of what was fixed and the final state of the navigation message data set.
 * PRSolve updated to support new Global Trop Model as part of existing Trop model option.

*Application Interface Changes*
 * bc2sp3 new option | --cs <sec>  Cadence of epochs in seconds (300s)
 * PRSolve updated option | Trop model <m> [one of Zero, Black, Saas, NewB, Neill, GG, GGHt, Global, with optional weather T(C),P(mb),RH(%)]

*New Applications*
 * core/apps/Rinextools/scanBrdcFile
 * core/apps/time/timediff
 * ext/apps/time/calgps_svg

*Updated Applications*
 * core/apps/time/calgps
 * ext/apps/geomatics/JPLeph/convertSSEph
 * ext/apps/geomatics/JPLeph/testSSEph
 * ext/apps/geomatics/relposition/DDBase

*Ext to Core Applicaton Migration (Tested and migrated with test code in appropriate folders)*
 * Rintools   - RinEdit, RinSum
 * checktools - rmwcheck, rowcheck, mnwcheck
 * difftools  - rinheaddiff, rmwdiff, rnwdiff, rowwdiff
 * filetools  - bc2sp3
 * mergetools - mergeRinMet, mergeRinNav, mergeRinObs
 * time       - calgps (modified) 

**SWIG Bindings & Python** 
 * Python test infrastructure enhancements.
 * Added python install information to the CMake configuration to identify where the python lib was installed.
 * Added SWIG template for vector with ObsID contents in swig/src/STLTemplates.i, since the data structure combination of std:vector and ObsID were inaccessible to python programs due to this combinatio not being included in the SWIG interface file.  


**Test Changes**

*New/Added Tests*
 * core/tests/checktools/CMakeLists.txt - rmwcheck, rowcheck, mnwcheck tests
 * core/tests/FileHandling/Binex_Attrs_T.cpp
 * core/tests/FileHandling/Binex_ReadWrite_T.cpp
 * core/tests/FileHandling/Rinex3Obs_FromScratch_t.cpp
 * core/tests/FileHandling/RinexMet_T.cpp
 * core/tests/filetools/CMakeLists.txt - bc2sp3 tests
 * core/tests/GNSSCore/CMakeLists.txt - TropModel tests
 * core/tests/GNSSEph/BrcClockCorrection_T.cpp
 * core/tests/GNSSEph/BrcKeplerOrbit_T.cpp
 * core/tests/GNSSEph/EngEphemeris_T.cpp
 * core/tests/GNSSEph/GPSEphemerisStore_T.cpp
 * core/tests/GNSSEph/NavID_T.cpp
 * core/tests/GNSSEph/OrbitEphStore_T.cpp
 * core/tests/mergetools/CMakeLists.txt - mergeRinMet, mergeRinNav, mergeRinObs tests
 * core/tests/NavFilter/CNavFilter_T
 * core/tests/positioning/CMakeLists.txt - PRSolve tests
 * core/tests/RefTime/TimeSystemCorr_T.cpp
 * core/tests/Rinextools/CmakeLists.txt - RinDump, RinEdit, RinSum, scanBrdcFile tests
 * core/tests/Rinextools/testScanBrdcFile.cmake
 * core/tests/time/CMakeLists.txt - timeconvert test
 * core/tests/time/CMakeLists.txt - Timediff tests
 * core/tests/TimeHandling/CMakeLists.txt - PosixTime test
 * core/tests/TimeHandling/IRNWeekSecond_T.cpp
 * core/tests/TimeHandling/PosixTime_T.cpp
 * core/tests/TimeHandling/TimeRange_T.cpp
 * core/tests/Utilities/StringUtils_T.cpp
 * ext/tests/geomatics/CMakeLists.txt - StatsFilter test
 * ext/tests/geomatics/StatsFilter_T.cpp
 * ext/tests/GNSSEph/CMakeLists.txt - OrbDataSys
 * ext/tests/GNSSEph/OrbAlm_T.cpp
 * ext/tests/GNSSEph/OrbSysStore_T.cpp
 * ext/tests/multipath/CMakeLists.txt - mpsolve tests
 * ext/tests/multipath/testmpsolve.cmake
 * ext_tests/GNSSEph/OrbSysGpsC_T.cpp
 * swig/tests/test_tropmodel.py

*Modified Test files*
 * core/tests/CommandLine/CommandOption_T.cpp
 * core/tests/CommandLine/CommandOptionParser_T.cpp
 * core/tests/FileDirProc/FileHunter_T.cpp
 * core/tests/FileDirProc/FileSpec_T.cpp
 * core/tests/FileDirProc/FileUtils_T.cpp
 * core/tests/FileHandling/Binex_Attrs_T.cpp
 * core/tests/FileHandling/Binex_ReadWrite_T.cpp
 * core/tests/FileHandling/FFBinaryStream_T.cpp
 * core/tests/FileHandling/Rinex3Obs_T.cpp
 * core/tests/FileHandling/RinexNav_T.cpp
 * core/tests/GNSSEph/PackedNavBits_T.cpp
 * core/tests/GNSSEph/SatID_T.cpp
 * core/tests/Math/Stats_T.cpp
 * core/tests/RefTime/TimeSystem_T.cpp
 * core/tests/time/CMakeLists.txt - timeconvert tests
 * core/tests/TimeHandling/ANSITime_T.cpp
 * core/tests/TimeHandling/CommonTime_T.cpp
 * core/tests/TimeHandling/GPSWeekSecond_T.cpp
 * core/tests/TimeHandling/GPSWeekZcount_T.cpp
 * core/tests/TimeHandling/JulianDate_T.cpp
 * core/tests/TimeHandling/MJD_T.cpp
 * core/tests/TimeHandling/SystemTime_T.cpp
 * core/tests/TimeHandling/UnixTime_T.cpp
 * core/tests/Utilities/StringUtils_T.cpp
 * core/tests/Utilities/ValidType_T.cpp
 * ext/tests/GNSSEph/OrbDataSys_T.cpp


**Truth Data Changes**

*New Truth Data*
 * data/EarthOrientation_SOFA.exp
 * data/EarthTides_IERS.exp
 * data/inputs/brdc0070.16n
 * data/inputs/brdc0080.16n
 * data/inputs/igs/
 * data/inputs/igs/cags1700.16o
 * data/inputs/igs/FAA100PYF_R_20161700100_15M_01S_MO
 * data/inputs/igs/faa1170b00.16o
 * data/inputs/igs/igs19016.sp3
 * data/inputs/igs/kerg1700.16o
 * data/inputs/igs/nklg170b00
 * data/inputs/igs/nrmg0150.16o
 * data/inputs/igs/osn31700.16o
 * data/inputs/igs/solo0150.16o
 * data/inputs/igs/sptu0150.16o
 * data/inputs/igs/UCAL00CAN_S_20161700100_15M_01S_MO
 * data/inputs/RinEdit_GPS_only
 * data/JPL_403eph_accuracy.exp
 * data/JPL_403eph_conversion.exp
 * data/JPL_405eph_accuracy.exp
 * data/JPL_405eph_conversion.exp
 * data/mergeRinMet_2.exp
 * data/mergeRinNav_2.exp
 * data/mergeRinObs_2.exp
 * data/outputs/RinDump_211Mixed.exp
 * data/outputs/RinDump_303_nrmg_BDStest.exp
 * data/outputs/RinDump_303_solo_BDStest.exp
 * data/outputs/RinDump_303_sptu_BDStest.exp
 * data/outputs/RinEdit_302merge.exp
 * data/outputs/RinEdit_302split1.exp
 * data/outputs/RinEdit_302split2.exp
 * data/outputs/RinEdit_302to211GPS.exp
 * data/outputs/RinEdit_302to211Mixed.exp
 * data/outputs/RinSum_obspath_v211.exp
 * data/outputs/RinSum_v11_nklg.exp
 * data/outputs/RinSum_v210_osn3.exp
 * data/outputs/RinSum_v211_cags.exp
 * data/outputs/RinSum_v211_kerg.exp
 * data/outputs/RinSum_v300_sptu_BDStest.exp
 * data/outputs/RinSum_v302_FAA1.exp
 * data/outputs/RinSum_v302_nrmg_BDStest.exp
 * data/outputs/RinSum_v303_solo_BDStest.exp
 * data/outputs/scanBrdcFile1-007.exp
 * data/outputs/scanBrdcFile1-008.exp
 * data/outputs/scanBrdcFile1-sum.exp
 * data/RinDump_Rinex2Input.exp
 * data/RinEdit_ValidOutput.exp
 * data/rinex3ObsTest_v211_CompleteR.exp
 * data/rinex3ObsTest_v211_MixedTest.exp
 * data/rinex3ObsTest_v211_ValidTest.exp
 * data/rinex3ObsTest_v302_CompleteR.exp
 * data/rinex3ObsTest_v302_MixedTest.exp
 * data/rinex3ObsTest_v302_ValidTest.exp
 * data/rnwdiff2.exp
 * data/StringUtils/hexDumpDataConfigTest_hexDump_1.exp
 * data/StringUtils/hexDumpDataConfigTest_hexDump_10.exp
 * data/StringUtils/hexDumpDataConfigTest_hexDump_11.exp
 * data/StringUtils/hexDumpDataConfigTest_hexDump_2.exp
 * data/StringUtils/hexDumpDataConfigTest_hexDump_3.exp
 * data/StringUtils/hexDumpDataConfigTest_hexDump_4.exp
 * data/StringUtils/hexDumpDataConfigTest_hexDump_5.exp
 * data/StringUtils/hexDumpDataConfigTest_hexDump_6.exp
 * data/StringUtils/hexDumpDataConfigTest_hexDump_7.exp
 * data/StringUtils/hexDumpDataConfigTest_hexDump_8.exp
 * data/StringUtils/hexDumpDataConfigTest_hexDump_9.exp
 * data/StringUtils/hexDumpDataStreamFlagTest_hexDump.exp
 * data/StringUtils/hexDumpDataTest_configHexDump.exp
 * data/StringUtils/hexDumpDataTest_hexDump.exp
 * data/StringUtils/hexDumpDataTest_printableMessage.exp
 * data/StringUtils/hexToAsciiTest_configHexDump.exp
 * data/StringUtils/hexToAsciiTest_hexDump.exp
 * data/StringUtils/hexToAsciiTest_printableMessage.exp
 * data/test_input.ddbase.opt_ok
 * data/test_input_ddbase.eop
 * data/test_input_ddbase_85412_new_044to045.15n
 * data/test_input_ddbase_85412_new_045.15o
 * data/test_input_ddbase_85412_old_045.15o
 * data/test_input_mpsolve.15n
 * data/test_input_mpsolve.15o
 * data/test_input_rinex_met_302.04m
 * data/test_input_sp3_nav_2015_200.sp3
 * data/test_output_mpsolve_l0.exp
 * data/test_output_mpsolve_l3000.exp
 * data/test_output_mpsolve_l3600.exp
 * data/test_output_mpsolve_mBAD.exp
 * data/test_output_mpsolve_mOK.exp
 * data/test_output_mpsolve_oe.exp
 * data/test_output_mpsolve_oea.exp
 * data/test_output_mpsolve_oec.exp
 * data/test_output_mpsolve_oed.exp
 * data/test_output_mpsolve_oen.exp
 * data/test_output_mpsolve_oenracd.exp
 * data/test_output_mpsolve_oer.exp
 * data/test_output_mpsolve_u0.exp
 * data/test_output_mpsolve_u30.exp
 * data/test_output_mpsolve_w10.exp
 * data/test_output_mpsolve_w17.exp
 * data/test_output_mpsolve_w30.exp
 * data/test_output_OrbAlm_T_GPS_LNAV.exp
 * data/test_output_OrbDataSys_T_GPS_CNAV.exp
 * data/test_output_OrbDataSys_T_GPS_LNAV.exp
 * data/testocean.blq
 * data/TropModel_GCAT.exp
 * data/TropModel_GG.exp
 * data/TropModel_GGHeight.exp
 * data/TropModel_Global.exp
 * data/TropModel_MOPS.exp
 * data/TropModel_NB.exp
 * data/TropModel_Saas.exp
 * data/TropModel_Simple.exp
 * data/TropModel_Zero.exp

*Updated Truth Data*
 * data/arlm2000.15n
 * data/arlm2000.15n
 * data/arlm2000.15n
 * data/arlm2001.15n
 * data/arlm200a.15n
 * data/arlm200b.15n
 * data/arlm200z.15n
 * data/bc2sp3_Same_1.exp
 * data/bc2sp3_Same_2.exp
 * data/bc2sp3_Same_3.exp
 * data/mergeRinNav_1.exp
 * data/mergeRinNav_2.exp
 * data/outputs/RinEdit_302merge.exp
 * data/outputs/RinEdit_302split1.exp
 * data/outputs/RinEdit_302split2.exp
 * data/PRSolve_Required.exp
 * data/PRSolve_Rinexout.exp
 * data/RinEdit_ValidOutput.exp -> data/outpus/RinEdit_Header.exp
 * data/rnwdiff1.exp
 * data/rnwdiff3.exp
 * data/rowdiff1.exp
 * data/rowdiff2.exp
 * data/rowdiff3.exp

*Deprecated Truth Data*
 * data/RinSum_ycode.exp


Fixes since v2.9
----------------

 * FileHunter file fix for Debian Build of GPSTk tests.
 * RinexObsHeader bug fix to handle multiple lines of continuation, as the original code assumed only one.
 * RinDump and RinEdit bug fixes, submitted by Brian Tolman via email.
 * Fixed home directory expansion for python user install.  Replaced tilde with $HOME as it wasn't always working.
 * Fixed messed up freq/indx lines in Rinex3 obs headers by adding an obs type to support GLONASS freq indexes.
 * Fixed build.sh script bug for building Doxygen API.
 * Fixed source packaging in BuildSetup.cmake by excluding the build, example, and ref directories from CPack.
 * Fixed conversions to/from BDT, added Jan 2017 leap second to table, cleaned up magic numbers in order to resolve a problem that was found in the determination of the UTC / BeiDou time offset
 * Fixed memory leak during clear() in OrbitEphStore; moved the implementation out of the header file.
 * RinEdit --DO option no longer also deletes the observation following the desired deletion.
 * Fixed return code in swig/gpstk/test_utils.py to be non-zero when there are uncaught exceptions thrown during tests.
 * Fixed several bugs and introduced additional tests to verify the functionality of RinEdit/RinSum/RinDump when processing RINEX files with Galileo data.
 * OrbAlmGen was modified to recognize and reject placeholder almanacs.
 * Fixed issue in GPSEphemerisStore in which it was not choosing correct ephemerides.  GPSEphemerisStore had been observed skipping over entire ephemerides when used to compute residuals via ObsRngDev.
 * Changes to Rinex3NavData and RinexSatID to keep RinDump from crashing when reading RINEX files containing IRNSS data.  
 * Fixed CMake scripts to build swig module directory prior to building the module, because when building using -j1 or on a single core machine, the build of the swig python module would fail due to a directory not having been created.
 * TUASSERT macros in TestUtil.hpp now catch exceptions and fail appropriately.
 * Fixed issue where mergeRinObs was failing on some RINEX files by allowing files with CR/LF Line Endings...thanks Windows.
 * Resolved the issue of EngEphemeris having no operator==
 * Fixed FileHunter::find() time system bug and added relevant unit tests
 * Fixed trivial bug in DiscFix configuration input
 * Fixed handling of wordless strings in StringUtils::words()/removeWords(), added unit tests, and updated documentation.
 * Fixed bug in Rinex3EphemeriStore::Initial/FinalTime() that allowed access of empty stores.
 * Fixed bug in AntennaStore.cpp in which TimeSystem of CommonTime() object needed to be set to TimeSystem::Any.
 * Updated incorrect TDB time String in TimeSystem.cpp as it was surely a mistake to make the string for Barycentric dynamic time (TDB, the time of the JPL ephemeris) "TRT", which is Turkey time.
 * Fixed windows tests of RinSum by dropping filesize of file output.
 * Fixed a bug in which CommandOptionNOf::which() never returns more than one option.  For example, if you're trying to use CommandOptionNOf to allow for combinations of options (e.g. timdiff in the MSN), the which() method never returns more than one option that was specified out of the group. That is if you have xOpt and yOpt in the NOf group, and the user uses both xOpt and yOpt, which() will only return one of them.
 * Fixed issue in which using StringUtils::prettyPrint does not handle new lines.  For example, if the text includes newlines, the new line appears to be handled as if it were a normal printable character and therefore the output is broken into strange segments.
 * Fixed small bug in GloEphemeris store in which bool was treated as double.
 * Fixed bug in SaasTropModel wet delay formula, also fixed references in comments.
 * NavID.hpp fix to for Windows warnings.
 * Changes to bc2sp3 test to fix test failures on Windows.
 * Fixed a bug in StatsFilter in which getStats() was not ignoring bad data beyond the end of the good data.
 * Fixed problem with PackedNavBits::operator<(). PackedNavBits::operator<() is used for sorting navigation message bits in some of the SGLTk NavFilter classes.  PackedNavBits::operator<() starts at the most significant bit and compares the bits one-by-one.  (side note:  if the two objects are not the same length, the shortest is regarded as the lesser of the two.)  The first object with a '0' bit is regarded as the lesser of the two. However, in the case of left < right if they started out equal, but then left had a '1' and right had a '0', it was not returning false immediately but continuing to scan.