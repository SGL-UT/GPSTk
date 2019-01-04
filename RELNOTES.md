GPSTk 2.11 Release Notes
========================

Updates since v2.10
------------------

**Build System and Test Suite**
 * Remove inadequately explained lib directory override in CMakeLists.txt
 * Added line to GPSTKConfig.cmake to allow dependent projects to find header files regardless of implicit or explicit declaration.
 * Test coverage was added on the core-build using gcov and gcovr
 * Support for GootleTest/GoogleMock added.
 * Updated Debian configuration files for complete debian package building capability
 * Travis-CI yaml script added for external Github CI autobuilds.  
 * Official deb packages are now build for master branch builds and available as artifacts.


**Gitlab CI**
 * Changes to .gitlab-ci.yml to fix SGLTk downstream portion of the GPSTk Gitlab CI/CD pipeline.    
 * Test coverage was added on the core-build using gcov and gcovr
 * Style compliance checking was added (as a new CI-Pipeline stage) using cpplint, but is not enforced.


**Library Changes**
 * SunEarthSatGeometry: Added OrbitNormalAttitude function back in, which was originally added in commit 78e29c30, but accidentally deleted in commit 148a0180.
 * Added test in core/lib/Utilities/StringUtils:change() that returns immediately if either input string is empty. REASON: Empty input causes no change, and so the loop (numChanges = max unsigned int) is essentially infinite. 
 * Added routines to core/lib/Utilities/stl_helpers.hpp that find the intersection of two vectors and the 'non-intersection'. NOTE: While there are STL routines that do this, they require sorting the vectors first, which is not good for my application (at least).
 * Make all geomatics tests in ext/tests/geomatics/CMakeLists.txt have the same label.
 * In core/lib/Geomatics/Namelist.* - Added operator+=(Namelist) for convenience. Found that randomize() does not do anything on Linux Debian (?!) so just replaced std:: call with simple code. Tiny improvements to the format of matrix/vector output.
 * Added debiasing before computing stats in core/lib/PosSol/PRSolution.hpp. REASON: Stats on the full PRS solution, which is an Earth-centered Earth-fixed position[3], often suffers numerically b/c the numbers are so large. This change simply debiases the numbers by the first value, so stats, in particular standard deviation, do not have noticeable numerical error.
 * Reverted core/lib/FileHandling/Rinex3NavData.cpp to output the xmitTime in the frame of the toe week (derived output week).  REASON: Originally, stored xmitTime in the frame of the Rinex3NavData::weeknum which was the week of sf1 HOWtime. output week was derived from weeknum and the difference between xmitTime and toe.  When the review was requested, stored xmitTime in the frame of Rinex3NavData::weeknum which was changed to be the toe week. I checked both the RINEX 2.11 (section 6.8) and 3.03 (section 6.13) specs and they agree with the change.
 * Added code to core/lib/GNSSEph/RationalizeRinex (used by core/apps/Rinextools/scanBrdcFile) to detect RINEX nav data sets tagged with the wrong SV PRN ID and to not copy such data sets to the output file. REASON: Latest IGS-generated merged RINEX nav (brdc) files contain data sets that are tagged with the incorrect PRN ID.
 * NavFilter updates/enhancements -  Add a nav filter for forced ordering of data.  Add methods for determining filter depth for buffer size computation. ITT receivers in particular do not always output nav subframes in time order or on time or with correct time stamps.
 * Added ObservationType for demodulator status codes, which will support changes to tools in SGLTk made in a future merge
 * ORD Refactor with CI Pipeline Changes - ord.cpp was exposed in the SWIG interface. Unit tests for coverage were written using GoogleTest/GoogleMock. Test coverage was measured on the core-build using gcov and gcovr. REASON: The ORD routines was considered general enough to add to GPSTk.  The project driving it is the HRTR Web Development effort.
 * Updated references for 54/18 and 34/18 and updated PRN 4 gap
 * Added LNAVOrderFilter for forcing time-order of legacy nav subframe data. REASON: When processing data from multiple receivers, the data may not be sent by the receivers at the same time.  This filter makes a "best effort" to enforce time ordering across multiple receivers going into the filter.
* Added sp3d file format support. REASON: We care about sp3d because the IGS MGEX precise orbit files moved from sp3c to sp3d in fall 2017.  Therefore, to support various multi-GNSS data experiments, we need to be able to access these files.
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
* Create a simple class for applying 10-bit week rollover corrections, with room to expand for other similar capabilities
* ext/lib/FileHandling/TimeNamedFileStream.hppnow creates directories for the files it outputs. REASON: ext/apps/rfw/rfw.cpp needed handholding to do its job. Now it doesn't.
* Adding L1C to ObsID and QZSS to various places. REASON: Need L1C support for upcoming GPS III launch.  QZSS already has a L1C broadcast, so it makes sense to look at that as well.
* Added method that returns a reference to PtoNMap. Used to generate csv format of the map. REASON: Want to generate a CSV form of svNumXRef, and this method is used for that.
* Added XvtStoreSatID shim class, which provides clear separation for the descendents of XvtStore that handle satellite data while avoiding any side effects for non-SatID XvtStore classes. REASON:The need for this has been apparent for a while, but the fact XvtStore is also implemented with classes other than SatID presented a problem. REASON: In several places, classes of XvtStore are used to store/retrieve satellite information. As more satellite systems are added and PRN space expands, there is a need to obtain a list of the satellites contained in a store in order to avoid having to do brute force searches over ranges of satellite systems and/or ranges of PRNs.   This is already implemented in OrbElemStore (a descendent of XvtStore, but not in other parallel stores (e.g. SP3EphemerisStore).
 These changes will impact the following classes, but hopefully only by adding functionality.
  * SP3EphemrisStore
  * PositionSatStore
  * TabularSatStore
  * GPSEphemerisStore
  * OrbElemStore

*New Library Classes*
 * core/lib/GNSSCore/GalileoEllipsoid.hpp
 * core/lib/NavFilter/CNav2SanityFilter.*
 * core/lib/NavFilter/LNavOrderFilter.*
 * core/lib/ORD/ord.*
 * core/lib/TimeHandling/TimeCorrection.*


**Application Changes**
 * core/apps/Rinextools/scanBrdcFile updated to detect RINEX nav data sets tagged with the wrong SV PRN ID and to not copy such data sets to the output file.
 * Update core/apps/checktools/rowcheck.cpp & core/apps/checktools/rnwcheck.cpp to use newer RINEX classes for rowcheck and rnwcheck to make them RINEX3 compatible

*Updated Applications*
 * core/apps/checktools/rnwcheck.cpp
 * core/apps/checktools/rowcheck.cpp
 * core/apps/Rinextools/scanBrdcFile

 
**SWIG Bindings & Python** 
 * Updated SWIG interface files for ORD refactor  

*New/Added SWIG Files*
 * swig/src/ord.i

**Test Changes**

*New/Added Tests*
 * core/tests/AppFrame/BasicFrameworkHelp_T.cpp
 * core/tests/GNSSEph/OrbElemStore_T.cpp
 * core/tests/GNSSEph/SP3EphemerisStore_T.cpp
 * core/tests/NavFilter/CNav2Filter_T.cpp
 * core/tests/ORD/GTestExperiments.cpp
 * core/tests/ORD/OrdMockClasses.hpp
 * core/tests/TimeHandling/TimeCorrection_T.cpp

*Modified Test files*
 * core/tests/AppFrame/CMakeLists.txt
 * core/tests/FileHandling/SP3_T.cpp
 * core/tests/GNSSEph/NavID_T.cpp
 * core/tests/NavFilter/NavFilterMgr_T.cpp
 * core/tests/ORD/OrdRegressionChecks_T.cpp
 * core/tests/ORD/OrdUnitTests_T.cpp
 * core/tests/testsuccexp.cmake
 * core/tests/TimeHandling/GPSWeekZcount_T.cpp
 * ext/test/geomatics/CMakeLists.txt
 * ext/tests/GNSSEph/OrbAlm_T.cpp
 * ext/tests/GNSSEph/OrbAlmStore_T.cpp

**Truth Data Changes**

*New Truth Data*
 * data/BasicFrameworkHelp_T_1.err.exp
 * data/BasicFrameworkHelp_T_1.exp
 * data/BasicFrameworkHelp_T_3.err.exp
 * data/BasicFrameworkHelp_T_3.exp
 * data/BasicFrameworkHelp_T_4.err.exp
 * data/BasicFrameworkHelp_T_4.exp
 * data/BasicFrameworkHelp_T_8.err.exp
 * data/BasicFrameworkHelp_T_8.exp
 * data/BasicFrameworkHelpReq_T_1.err.exp
 * data/BasicFrameworkHelpReq_T_1.exp
 * data/test_input_SP3d.sp3 
 * data/test_output_rinex3_clock_TestR3HeaderOutput.exp


*Updated Truth Data*
 * data/outputs/RinSum_obspath_v211.exp 
 * data/outputs/RinSum_v211_kerg.exp
 * data/outputs/RinSum_v211_nklg.exp
 * data/outputs/RinSum_v302_FAA1.exp


Fixes since v2.10
----------------

 * Fixed a bug in core/lib/Math/Stats.hpp that produced unpredictable results when trying to add an empty object.
 * int -> unsigned int changes in ext/lib/Geomatics/StatsFilter.hpp
 * Freed memory returned by core/lib/GNSSEph/OrbitEphStore::addToList and updated comments for that method.  This fixed memory leaks in core/lib/GNSSEph/GPSEphemerisStore::addToList and core/lib/GNSSEph/Rinex3EphemerisStore::addToList.
 * Corrected prettying print of eph health bits in core/lib/GNSSEph/EngEphemeris.cpp
 * Fixed incorrect end time for 54/18 in core/lib/GNSSCore/SVNumXRef.cpp
 * Fixing bug that introduced false positive on Downstream GPSTk Core build
 * Updated yuma reader to work without newline at end of file. REASON: Needed flexibility to handle files with and without newlines at the end without the reader breaking.  
 * Replaced ICD-GPS-211 with IS-GPS-200 in several comments