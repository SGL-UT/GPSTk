GPSTk 5.0.0 Release Notes
========================

 * Introduced in this version is prelimiary support for Rinex 3.04.
   NOTE: Rinex 3.04 support is not fully in compliance with "9.1 Phase Cycle Shifts", although that does not affect any of our use cases.
 * This version also captures several other incidental API changes.

Updates since v4.0.0
---------------------

**Build System and Test Suite**
  * Add tests for computeXvt and getSVHealth with unhealthy satellites
  * Update MJD/JD timeconvert tests to be able to pass under windows.
  * Remove FileHunter_T failing test from Windows build in preparation for future deprecation of the FileHunter class.
  * Update rstats testing to use portable ctest mechanisms instead of the python script, and split the truth data accordingly.
  * Update Yuma_T test to use the most modern cmake test script and df_diff to account for minor differences in floating point least significant digits.
  * Fix RINEX reference data for tests containing the "Signal Strenth" typo, or remove it for RINEX 2 tests.
  * Add FileUtils/FileUtils_T comments explaining debian 7 test failures.

**Gitlab CI**
  * Update CI to retain, as CI artifacts, output from failed tests.

**Library Changes**
  * Add ObsID Add methods to convert between enumeration and string and test them.
  * Add NavID Add methods to convert between enumeration and string and test them.
  * Update OrbitEphStore to have more information when a potentially corrupt ephemeris is added.
  * Deprecate SVNumXRef in favor of SatMetaDataStore
  * Add FileSpecFind as an eventual replacement for FileHunter
  * Update CommandOptionParser to remove the .exe file extension when printing help under Windows.
  * Modify installation script to create site-packages if it doesn't exist.
  * Update Rinex3ObsHeader to use a nested class called "Flags" for header field management, replacing the bit field.
  * Add SWIG bindings to support the nested classes within Rinex3ObsHeader.
  * Update Position::transformTo,asGeodetic,asECEF to return a reference.
  * Update CommandOption classes to return a const reference instead of a copy of a vector.
  * Add BasicFramework documentation.
  * Update RinSum to have better diagnostics when there is an invalid Rinex header
  * Move a collection of tests and code from ext to core.
  * Update RinDump to support all current (as of RINEX 3.04) codes
  * Update RinSum to improve support aux headers over simple comment support.
  * Update PRSolve to input multiple GNSS options, and for multi-GNSS solution, including one Rx clock per system.
  * Update Rinex3ObsData to support channel number and ionospheric delay data.
  * Remove frequency/wavelength constants from GNSSconstants.hpp and put them in DeprecatedConsts.hpp
  * Add FreqConsts.hpp to replace the deprecated constants with new names based on ICD naming instead of RINEX.
  * Add ObsID support for numerous previously unsupported codes.
  * Add RinexObsID support for decoding channel number and ionospheric delay pseudo-observables.
  * Update OrbitEph::adjustValidity to subtract two hours only for GPS.
  * Update PRSolution for multi-GNSS solution, including one Rx clock per system.
  * Update PreciseRange to input frequencies as well as GNSS.

Fixes since v4.0.0
--------------------
  * Update SWIG files so that gpstk::Exception is treated as a Python Exception
  * Fix Rinex3ObsHeader::Fields SWIG bindings.
  * Fix decimation bug Rinex3ObsLoader corrected decimation logic
  * Fix RinexSatID/SP3SatID to properly handle QZSS and GEO satellite IDs over 100
  * Update FileSpec to use WIN32 precompiler macro instead of _WIN32 to get the correct file separator.
  * Fix SWIG bindings to use Python-list to C-array typemaps in gpstk_typemaps.i
  * Fix Rinex3ClockData/Rinex3ClockHeader to decode systems using existing classes.
  * Fix Rinex3NavData to use a sane begin time for QZSS nav.
  * Fix QZSEphemeris to use a sane begin time for QZSS nav.


Removed Code due to Deprecation
-------------------------------
     data/test_rstats.exp
     ext/apps/geomatics/rstats_test/CMakeLists.txt
     ext/apps/geomatics/rstats_test/rstats_T.py
     ext/tests/GPSOrbElemStore/GenSyntheticCNAVData.cpp
     ext/tests/GPSOrbElemStore/ReadSyntheticCNAVData.cpp
     ext/tests/GPSOrbElemStore/xOrbElemCNAV.cpp
     ext/tests/GPSOrbElemStore/xOrbElemCNAV2.cpp
     ext/tests/GPSOrbElemStore/xOrbElemICE.cpp
     ext/tests/GPSOrbElemStore/xTestRationalize.cpp
     ext/tests/HelmertTransform/HelmertTransform.cpp
     ext/tests/HelmertTransform/HelmertTransform.hpp
     ext/tests/HelmertTransform/Jamfile
     ext/tests/HelmertTransform/xHelmertTransform.cpp
     ext/tests/HelmertTransform/xHelmertTransform.hpp
     ext/tests/HelmertTransform/xHelmertTransformM.cpp
     ext/tests/PowerSum/Jamfile
     ext/tests/PowerSum/PowerSum.cpp
     ext/tests/PowerSum/PowerSum.hpp
     ext/tests/RungeKutta4/Jamfile
     ext/tests/RungeKutta4/RungeKutta4.cpp
     ext/tests/RungeKutta4/RungeKutta4.hpp
     ext/tests/RungeKutta4/makefile
     ext/tests/RungeKutta4/pendulum.hpp
     ext/tests/RungeKutta4/xRungeKutta4.cpp
     ext/tests/RungeKutta4/xRungeKutta4.hpp
     ext/tests/RungeKutta4/xRungeKutta4M.cpp
     ext/tests/SEM/CMakeLists.txt
     ext/tests/SEM/sem.txt
     ext/tests/SEM/sem387.txt
     ext/tests/Yuma/CMakeLists.txt
     ext/tests/gpsNavMsg/Checks/AfterHalfweekchange.txt
     ext/tests/gpsNavMsg/Checks/BeforeHalfweekchange.txt
     ext/tests/gpsNavMsg/Checks/BrcClockCorrection_Test_June_16_2011_4pm.txt
     ext/tests/gpsNavMsg/Checks/BrcKeplerOrbit_Test_June_16_2011_4pm.txt
     ext/tests/gpsNavMsg/Checks/ClockCorrTest.txt
     ext/tests/gpsNavMsg/Checks/OrbTest.txt
     ext/tests/gpsNavMsg/Checks/PackedNavBits_Output.txt
     ext/tests/gpsNavMsg/Checks/xBrcClkCorr_output_01.txt
     ext/tests/gpsNavMsg/Checks/xBrcKeplerOrbit_withsetFrame.out.txt
     ext/tests/gpsNavMsg/Checks/xCEC.txt
     ext/tests/gpsNavMsg/Checks/xCNAVClock_Output.txt
     ext/tests/gpsNavMsg/Checks/xCNAVEphemeris_Output.txt
     ext/tests/gpsNavMsg/Checks/xPackedNavBits_Output.txt
     ext/tests/gpsNavMsg/GenSyntheticCNAVData.cpp
     ext/tests/gpsNavMsg/Jamfile
     ext/tests/gpsNavMsg/Logs/BrcClkCorr_Output
     ext/tests/gpsNavMsg/Logs/BrcClkCorr_Truth
     ext/tests/gpsNavMsg/Logs/BrcKeplerOrbit_Output
     ext/tests/gpsNavMsg/Logs/BrcKeplerOrbit_Truth
     ext/tests/gpsNavMsg/Logs/CNAV2PNB_Output
     ext/tests/gpsNavMsg/Logs/CNAV2PNB_Truth
     ext/tests/gpsNavMsg/Logs/CNAVClockPNB_Output
     ext/tests/gpsNavMsg/Logs/CNAVClockPNB_Truth
     ext/tests/gpsNavMsg/Logs/CNAVClock_Output
     ext/tests/gpsNavMsg/Logs/CNAVClock_Truth
     ext/tests/gpsNavMsg/Logs/CNAVEphemerisPNB_Output
     ext/tests/gpsNavMsg/Logs/CNAVEphemerisPNB_Truth
     ext/tests/gpsNavMsg/Logs/CNAVEphemeris_Output
     ext/tests/gpsNavMsg/Logs/CNAVEphemeris_Truth
     ext/tests/gpsNavMsg/Logs/EngEphemeris_Output
     ext/tests/gpsNavMsg/Logs/EngEphemeris_Truth
     ext/tests/gpsNavMsg/Logs/Output
     ext/tests/gpsNavMsg/Logs/PackedNavBits_Output
     ext/tests/gpsNavMsg/Logs/PackedNavBits_Truth
     ext/tests/gpsNavMsg/Logs/Truth
     ext/tests/gpsNavMsg/makefile
     ext/tests/gpsNavMsg/xBrcClockCorrection.cpp
     ext/tests/gpsNavMsg/xBrcClockCorrectiongpsNavMsg.cpp
     ext/tests/gpsNavMsg/xBrcClockCorrectiongpsNavMsg.hpp
     ext/tests/gpsNavMsg/xBrcKeplerOrbit.cpp
     ext/tests/gpsNavMsg/xBrcKeplerOrbitgpsNavMsg.cpp
     ext/tests/gpsNavMsg/xBrcKeplerOrbitgpsNavMsg.hpp
     ext/tests/gpsNavMsg/xCNAV2EphClk.cpp
     ext/tests/gpsNavMsg/xCNAV2EphClockPNB.cpp
     ext/tests/gpsNavMsg/xCNAV2PNBgpsNavMsg.cpp
     ext/tests/gpsNavMsg/xCNAV2PNBgpsNavMsg.hpp
     ext/tests/gpsNavMsg/xCNAV2gpsNavMsg.cpp
     ext/tests/gpsNavMsg/xCNAV2gpsNavMsg.hpp
     ext/tests/gpsNavMsg/xCNAVClock.cpp
     ext/tests/gpsNavMsg/xCNAVClockPNB.cpp
     ext/tests/gpsNavMsg/xCNAVClockPNBgpsNavMsg.cpp
     ext/tests/gpsNavMsg/xCNAVClockPNBgpsNavMsg.hpp
     ext/tests/gpsNavMsg/xCNAVClockgpsNavMsg.cpp
     ext/tests/gpsNavMsg/xCNAVClockgpsNavMsg.hpp
     ext/tests/gpsNavMsg/xCNAVEphemeris.cpp
     ext/tests/gpsNavMsg/xCNAVEphemerisPNB.cpp
     ext/tests/gpsNavMsg/xCNAVEphemerisPNBgpsNavMsg.cpp
     ext/tests/gpsNavMsg/xCNAVEphemerisPNBgpsNavMsg.hpp
     ext/tests/gpsNavMsg/xCNAVEphemerisgpsNavMsg.cpp
     ext/tests/gpsNavMsg/xCNAVEphemerisgpsNavMsg.hpp
     ext/tests/gpsNavMsg/xEngEphemerisgpsNavMsg.cpp
     ext/tests/gpsNavMsg/xEngEphemerisgpsNavMsg.hpp
     ext/tests/gpsNavMsg/xgpsNavMsgM.cpp
