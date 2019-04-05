GPSTk 2.11.2 Release Notes
========================

Updates since v2.11.1
------------------

**Gitlab CI**
 * Updating the YML file for Gitlab CI builds to add Solaris, OSX, and Windows in as Nightly-only builds.  This means that a normal continuous gitlab ci pipeline will only build for debian and redhat. REASON: While Windows, OSX, and Solaris are important and supported platforms, they consistently provide issues when ran in a continuous pipeline and are of less priority than Redhat and Debian. Currently, CDash was used for nightly builds, but migrating everything to a single gitlab platform is best.

**Library Changes**
 * Initialized Rinex3NavHeader class variables to allow comparisons to work better with optional fields.
 * Added L1C RINEX character codes to ObsID data structure as ObsIDInitializer did not provide character code translation for the special case of L1 carrier and CD / CP / CD+CP ranging code. REASON: Writing L1C data to a RINEX file was not possible.
 * Needed additional interfaces to query the OrbSysStore in order to: 
  * Know which NavIDs are in a store. 
  * Know which GNSS systems are in a store. 
  * Obtain a list<const OrbDataSys*> of all messages in the store that correspond to a given NavID/UID, regardless of the SV that transmitted them.
  * REASON: Existing interfaces returned a list of objects for a given SV or a given time range.  Downstream applications needed a list of all objects of a given data type (e.g., UTC corrections) in the store. The interface was extended with a new method, but all existing interfaces were maintained unchanged.
  * Provided update to correctly differentiate between BeiDou D1 and D2 data, as per Issue 374.  REASON: Can't do this downstream as the differentiation is detected by data rates and lost by the time we reach VUB files except for the NavID in the header. Also made OrbElem.svXvt() virtual to allow downstream override for unusual satellite systems.

**Application Changes**
 * 1. Changed all of the Rinex usage to Rinex3 within mergeRinNav.cpp & 2. Changed the .exp files used by mergeRinNav.cpp to expect one less significant figure in the header. REASON: Changes for (1) is needed so that the result would be written in scientific notation instead of D notation as per requested in MSN-5790.  Changes for (2) is needed because increasing the amount of significant figures that Rinex3 headers writes out ensures that the new version of mergeRinNav passes its application tests. However, it causes issues in other tests. Changing to the expected files allows mergeRinNav to pass its test and other tests to pass. Also, the creation of the new mergeRinNav_1.exp and mergeRinNav_2.exp use the same input.


*Updated Applications*
 * core/apps/mergetools/mergeRinNav.cpp

**SWIG Bindings & Python** 
 * A hard-coded value in FileIO.i is changed so that non-GPS RINEX data can be written with the RINEX3 classes
 * Disabled rpath for swig python binaries by forcing CMake to not include RPATH arguments when building SWIG binaries.  REASON: This integrates better with our isolated python environments. (e.g. conda)

**Test Changes**

*New/Added Tests*
 * ext/tests/GNSSEph/OrbSysStore_T.cpp
 * core/tests/FileDirProc/FileHunter_T.cpp

**Truth Data Changes**

*Modified Truth Data*
 * data/mergeRinNav_1.exp
 * data/mergeRinNav_2.exp
 * data/test_output_OrbDataSys_T_GPS_CNAV.exp
 * data/test_output_OrbDataSys_T_GPS_LNAV.exp

Fixes since v2.11.1
----------------
 * Modified the swig build to include the gpstk version as a top-level variable in the python module as installed tools could not determine version.
 * Fixed FileHunter::find() time system bug and added relevant unit tests.
 * Fixed Issue 375 (erroneously treating files as directories) and mostly addressed Issue 229 (added progressive time filtering for better performance).  Also performed some general code cleanup and improved exception handling and doxygen.