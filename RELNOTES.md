GPSTk 2.12.2 Release Notes
========================

Updates since v2.12.1
---------------------

**Build System and Test Suite**
 * Updated build code to build static libraries on Windows, Static and Dynamic on all other platforms.

**Gitlab CI**
 * Updating the CI Pipeline to use docker workers rather than VM workers. REASON: Docker images are more isolated and less accidental state carried between pipelines.


**Library Changes**
 * Updated SVN-PRN References - Updating PRN 4 assignments. REASON: PRN 4 was taken away from SVN 36 and later re-assigned to SVN 74
 * Added atmospheric loading to geomatics library. Includes a store for coefficients for multiple sites, a computeDisplacement routine and a new test in ext/apps/geomatics/iers_test/test_tides.cpp. REASON: This is needed in precise positioning, PPP and differential
 * Change in core/lib/GNSSEph/OrbElemStore findOrbElem selection logic. REASON: In the event the time of interest corresponds EXACTLY to a key in the map, it will now return that element instead of the previous element.   The previous logic was an attempt to more closely replicate the user experience by delaying cutovers between message sets until after transmission was complete.   However, this has proven to be confusing to users and would be very difficult to extend generally across all multi-GNSS message structures.

*New Library Classes*
 * ext/lib/Geomatics/AtmLoadTides.*

**SWIG Bindings & Python**
 * Added a flag to the gpstk setup.py file to ensure that the python module is installed as an egg directory. REASON: Some users were reporting problems with the python installation that traced back to the fact that the gpstk python module was being installed as a compressed egg file rather than an egg directory. Apparently, if the zip_safe flag is not specified, SetupTools will attempt to guess the best solution and some users were seeing it choose the archive file rather than directory. Setting the flag to false, should cause all users to see the expanded directory.
 * Python 3.6 Functionality has been added, while still supporting Python 2.7. REASON: Python 2.7 will be deprecated as of January 2020


**Test Changes**

*New/Updated Tests*
 * core/tests/FileHandling/Rinex3Obs_T.cpp
 * core/tests/AppFrame/CMakeLists.txt
 * core/tests/GNSSCore/CMakeLists.txt
 * core/tests/Rinextools/CMakeLists.txt
 * core/tests/checktools/CMakeLists.txt
 * core/tests/difftools/CMakeLists.txt
 * core/tests/filetools/CMakeLists.txt
 * core/tests/mergetools/CMakeLists.txt
 * core/tests/positioning/CMakeLists.txt
 * core/tests/testsuccexp.cmake
 * ext/apps/geomatics/rstats_test/CMakeLists.txt
 * ext/tests/GNSSEph/CMakeLists.txt
 * ext/tests/geomatics/CMakeLists.txt
 * ext/tests/geomatics/CMakeLists.txt


**Truth Data Changes**

*Updated Truth Data*
 * data/EarthTides_IERS.exp
 * data/testocean.blq

*New Data Files*
 * data/mixed211.05o
 * data/testatm.atl


Fixes since v2.11
--------------------
 * Fixed Issue 375 - FileHunter cannot follow links.
 * Fixed Issue 394 - Rinex3ObsData does not properly handle header records - When processing the sample data in the RINEX 2.11 document (mixed obs), I found that Rinex3ObsData was not reading header records properly when encountering epoch flag 4 ("header records follow").  Instead, the header reacords are being attached to the subsequent object read.
 * Fixed Issue 399 - Xvt.hpp does not compile on Debian 8 with HealthStatus:: on line 102
 * Fixed an issue with PRSolve - which failed when given a dual frequency solution descriptor where code1 is valid only on freq1, and code2 is valid only on freq2. Example: GAL:15:BI
