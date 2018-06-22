GPSTk 2.10.3 Release Notes
========================

Updates since v2.10.2
----------------------

**Library Changes**
 * Adding Ellipsoid model for Galileo. REASON: Required for processing Galileo orbits
 * Change documentation of ObsID phase units. REASON: Most of the defined file formats and associated readers did not respect the prior documentation for phase observations. This changes the documentation to be consistent with the usage in (at least) MDP, NovAtel, RINEX, and MDH.
 * Added ability to use getXvt() calls on unhealthy SVs in XvtStore objects returned by EphReader. Changes include: Migrated onlyHealthy, getOnlyHealthyFlag(), and setOnlyHealthyFlag() from OrbitEphStore to XvtStore; Made sure that default behavior for OrbitEphStore and OrbElemStore was retained; Changed implementation of health status in GloEphemerisStore. REASON: The changes were made since there is a need to be able to call getXvt() on the unhealthy SVs in XvtStore objects returned by EphReader.
 * Adding/Clarifying support for Galileo E5a/E5b in ObsID.  Adding Galileo F/NAV and I/NAV to NavID. REASON: Need these signals represented in the signal processing infrastructure. Part of Issue 341 - Update Galileo Signal Support.
 * Ensured full initialization of Stats objects and fixed a memory leak in OrbitEph::timeDisplay. REASON: This change ensures full initialization of Stats objects to eliminate undefined behavior.  Also, this fix contains a fix for a small memory leak in OrbitEph::timeDisplay.
 * ORD Calculation Fixes - addressed defects in the new ORD Calculation discovered by Jason Vestuto in his testing. REASON: The calculation results were incorrect.
 * Added a base class for help-like command-line options with special processing in CommandOptionParser and BasicFramework. REASON: Too many applications in the MSN have needed this, and this is a better solution than trying to figure out if these help-like options were used after all the normal command-line option parsing.
 * Restored Missing GPSWeekZcount Math. REASON: When the time code was migrated to CommonTime/TimeTag implementations, several extremely useful methods were discarded from the GPSWeekZcount class.
 * Relaxed fit interval and health restrictions on ext/GNSSEph/OrbAlmStore. REASON:  Existing code didn't address existing use cases.
 * Added recognition of Galileo antenna offsets to increase capability. REASON: Several other GNSSs are already implemented.   This is adding yet another GNSS in the same pattern. 

*New Library Classes*
 * core/lib/GNSSCore/GalileoEllipsoid.hpp

**Test Changes**

*New/Added Tests*
 * core/tests/AppFrame/BasicFrameworkHelp_T.cpp

*Modified Test files*
 * core/tests/GNSSEph/NavID_T.cpp
 * core/tests/ORD/OrdRegressionChecks_T.cpp
 * core/tests/ORD/OrdUnitTests_T.cpp
 * core/tests/AppFrame/CMakeLists.txt
 * core/tests/testsuccexp.cmake
 * core/tests/TimeHandling/GPSWeekZcount_T.cpp
 * ext/tests/GNSSEph/OrbAlmStore_T.cpp

**Truth Data Changes**

*New Truth Data*
 * data/BasicFrameworkHelpReq_T_1.err.exp
 * data/BasicFrameworkHelpReq_T_1.exp
 * data/BasicFrameworkHelp_T_1.err.exp
 * data/BasicFrameworkHelp_T_1.exp
 * data/BasicFrameworkHelp_T_3.err.exp
 * data/BasicFrameworkHelp_T_3.exp
 * data/BasicFrameworkHelp_T_4.err.exp
 * data/BasicFrameworkHelp_T_4.exp
 * data/BasicFrameworkHelp_T_8.err.exp
 * data/BasicFrameworkHelp_T_8.exp

*Updated Truth Data*
 * data/outputs/RinSum_obspath_v211.exp 
 * data/outputs/RinSum_v211_kerg.exp
 * data/outputs/RinSum_v211_nklg.exp
 * data/outputs/RinSum_v302_FAA1.exp

Fixes since v2.10.2
-------------------
 * Updated yuma reader to work without newline at end of file. REASON: Needed flexibility to handle files with and without newlines at the end without the reader breaking.   




