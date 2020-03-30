GPSTk 4.0.0 Release Notes
========================

 * This major version number release was due to some API breaking call signature changes.

Updates since v3.1.0
---------------------

**Gitlab CI**
  * Conda packages are now correctly preserved as build artifacts.
  * Master branch now run both build & test on debian 7, debian 8, and debian 10.

**Library Changes**
  * Changed a calling signature in OrbElemRinex
  * Add string conversions to-from SatID.SatelliteSystem
  * Remove exception specifications from function/method prototypes and definitions
  * SVNumXRef - PRN 18 removed from SVN 34 and assigned to SVN 75

Fixes since v3.1.0
--------------------
 * Fix to recognized QZSS navigation message data set cutovers.
 * Fix an installation bug when using versioned header directories.
 * Fix slight errors in svXvt velocity computation.
 * Update svXvt to set Xvt.health when available.
 * Fix BrcKeplerOrbit to use relativity correction and initial URE value of 0.
 * Fix AlmOrbit to add missing final value of G.
 * Fix BDSEphemeris to use GEO equations only above 7 degrees.
 * Change OrbitEph to use CGCS2000 ellipsoid for BeiDou when computing relativity correction
 * Fix bug in timeconvert app breaking after year 2038
 * Fix incorrect interval in output header from RinEdit if thinning option is used.
 * Fix memory leak during OrbElmStore deallocation.

Removed Code due to Deprecation
-------------------------------
     NEWS.md
