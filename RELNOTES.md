GPSTk 2.10.1 Release Notes
========================

Updates since v2.10
----------------------

**Build System and Test Suite**
 * Remove inadequately explained lib directory override in CMakeLists.txt
 * Added line to GPSTKConfig.cmake to allow dependent projects to find header files regardless of implicit or explicit declaration.

**Gitlab CI**
 * Changes to .gitlab-ci.yml to fix SGLTk downstream portion of the GPSTk Gitlab CI/CD pipeline.    

**Library Changes**
 * SunEarthSatGeometry: Added OrbitNormalAttitude function back in, which was originally added in commit 78e29c30, but accidentally deleted in commit 148a0180.
 * Added test in core/lib/Utilities/StringUtils:change() that returns immediately if either input string is empty. REASON: Empty input causes no change, and so the loop (numChanges = max unsigned int) is essentially infinite. 
 * Added routines to core/lib/Utilities/stl_helpers.hpp that find the intersection of two vectors and the 'non-intersection'. NOTE: While there are STL routines that do this, they require sorting the vectors first, which is not good for my application (at least).
 * Make all geomatics tests in ext/tests/geomatics/CMakeLists.txt have the same label.
 * In core/lib/Geomatics/Namelist.* - Added operator+=(Namelist) for convenience. Found that randomize() does not do anything on Linux Debian (?!) so just replaced std:: call with simple code. Tiny improvements to the format of matrix/vector output.
 * Added debiasing before computing stats in core/lib/PosSol/PRSolution.hpp. REASON: Stats on the full PRS solution, which is an Earth-centered Earth-fixed position[3], often suffers numerically b/c the numbers are so large. This change simply debiases the numbers by the first value, so stats, in particular standard deviation, do not have noticeable numerical error.
 * Reverted core/lib/FileHandling/Rinex3NavData.cpp to output the xmitTime in the frame of the toe week (derived output week).  REASON: Originally, stored xmitTime in the frame of the Rinex3NavData::weeknum which was the week of sf1 HOWtime. output week was derived from weeknum and the difference between xmitTime and toe.  When the review was requested, stored xmitTime in the frame of Rinex3NavData::weeknum which was changed to be the toe week. I checked both the RINEX 2.11 (section 6.8) and 3.03 (section 6.13) specs and they agree with the change.

*Modified Test files*
 * ext/test/geomatics/CMakeLists.txt


Fixes since v2.10
-----------------
 * Fixed a bug in core/lib/Math/Stats.hpp that produced unpredictable results when trying to add an empty object.
 * int -> unsigned int changes in ext/lib/Geomatics/StatsFilter.hpp
 * Freed memory returned by core/lib/GNSSEph/OrbitEphStore::addToList and updated comments for that method.  This fixed memory leaks in core/lib/GNSSEph/GPSEphemerisStore::addToList and core/lib/GNSSEph/Rinex3EphemerisStore::addToList.

