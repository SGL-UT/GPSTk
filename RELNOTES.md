GPSTk 6.0.0 Release Notes
========================

 * This version contains bug fixes required by a downstream dependency.  
 * The major version number was incremented because of some non-backward compatible changes 
   to function method definitions.

Updates since v5.0.1
---------------------
  * Update Ashtech and other classes to prevent SWIG from generating bad code.
  * Update CommonTime::setTimeSystem to return a reference to itself.
  * Update FileFilterFrame to use FileSpecFind instead of FileHunter.
  * Update FileFilterFrameWithHeader to use FileSpecFind instead of FileHunter.
  * Update FileSpec for a more optimal implementation of hasField
  * Update AntexData Add BeiDou-3, QZSS, NAVIC satellite types
  * Update SVNumXRef Deassign PRN23 from SVN60
  * Update SVNumXRef Add SVN76/PRN23
  * Update existing uses of doub2for etc. to use FormattedDouble.
  * Update test data to use fortran format numbers in RINEX nav data.
  * Update and Adding swig support for IonoModel and Nav Reader
  * Update SWIG bindings to fix memory leaks for Matrix.

**Build System and Test Suite**
  * Update swig CMakeLists.txt to preclude setting RUNPATH in _gpstk.so thus preventing it from pointing to the 
    wrong libgpstk.so

**Gitlab CI**
  * Update gitlab-ci.yml to declare a registry associated with each image.
  * Update CI Pipeline to be more adept at producing downloadable packages.
  * Add CI testing on Redhat 7 with SWIG 3 installed.

**Library Changes**
  * Add Rinex3ObsHeader accessors for wavelengthFactor in support of python.
  * Add FileSpec methods hasTimeField and hasNonTimeField.
  * Add Filter class to FileSpecFind to support matching of sets of values.
  * Add FormattedDouble as a more versatile replacement for doub2for, for2doub, etc.

Fixes since v5.0.1
--------------------
  * Fix SWIG vector constructor to take a list of integers
  * Fix FileSpecFind to handle more use cases under windows.
  * Fix FileSpecFind to support both forward slash and backslash file separators under windows.
  * Fix AntennaStore BeiDou phase center
  * Fix RinDump to eliminate a memory leak.
  * Fix rinheaddiff to eliminate a memory leak.
  * Fix PRSolve to eliminate a memory leak.
  * Fix FFTextStream to eliminate an invalid memory read.
  * Fix RinDump to eliminate a memory leak.
  * Fix OrbitEph to initialize all data members.
  * Fix RationalizeRinexNav to eliminate a memory leak.
  * Fix BasicFrameworkHelp_T to eliminate a memory leak.
  * Fix CommandOption_T to eliminate a memory leak.
  * Fix Rinex3Obs_FromScratch_t.cpp to eliminate a memory leak.
  * Fix CNav2Filter_T to eliminate a memory leak.
  * Fix CNavFilter_T to eliminate a memory leak.
  * Fix dfix to eliminate use of uninitialized variables.
  * Fix rstats to eliminate a memory leak.
  * Fix OrbAlmStore to eliminate a memory leak.
  * Fix OrbSysStore to eliminate a memory leak.
  * Fix WindowFilter to eliminate use of uninitialized variables.
  * Fix gdc::getArcStats to eliminate a memory leak.
  * Fix RinexObsHeader to eliminate use of uninitialized variables.
  * Fix GlobalTropModel to eliminate use of uninitialized variables.
  * Fix BDSEphemeris to eliminate use of uninitialized variables.
  * Fix OrbElem to eliminate use of uninitialized variables.
  * Fix TimeRange_T to eliminate dereferencing of unallocated memory pointers.
  * Fix FFBinaryStraem to eliminate use of uninitialized variables.
  * Fix YDSTime to eliminate use of uninitialized variables.
  * Fix Matrix destructors to work under antique Debian 7