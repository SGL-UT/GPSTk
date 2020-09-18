GPSTk 7.0.0 Release Notes
========================

 * This version addresses enumeration changes used by downstream dependencies in sgltk, mdhtk, mdhtk-python, pysgltk
 * It contains deprecation of python bindings (swig support) for rhel7 and deb7/8.
 * Additionally, bug fixes and library changes were implemented.

Updates since v6.0.0
---------------------
  * Update OceanLoadTides.hpp Add reference to SPOTL in doxygen comments
  * Refactor ObsID and RinexObsID initialization of containers (tcDesc, etc.) to use C++11 syntax and eliminate the use of a singleton initializer.
 
**Build System and Test Suite**
  * Dropping Python Swig binding support for RHEL7/DEB7
  * Refactor debian8 remove python bindings
  * Refactor debian dpkg-buildpackage Update control files to remove python bindings

**Gitlab CI**
  * Add CODEOWNERS file.
  * Refactor gpstk pipeline downstream jobs
  * Add Documentation Artifact to CI Pipeline

**Library Changes**
  * Add SatMetaDataStore findSatByFdmaSlot
  * Add EnumIterator class to provide the ability to iterate over the above enum classes.
  * Refactor Yuma/SEM file support back into ext
  * Refactor enumerations in TimeSystem, ReferenceFrame, IERSConvention, SatID, NavID and ObsID (SatelliteSystem, NavType, ObservationType, CarrierBand, TrackingCode) to use strongly typed enumerations and move them outside the scope of those classes.
  * Rename TrackingCode enumerations to better support codes that RINEX does not by using names based on the ICDs rather than what RINEX uses.
  * Refactor swig bindings for enums to use similar naming conventions between C++ and python (e.g. gpstk::TrackingCode::CA in C++ and gpstk.TrackingCode.CA in python)
  * Move RINEX-isms (such as the string constructor for decoding RINEX obs IDs) in ObsID into RinexObsID where they belong.
  * Refactor ObsID and RinexObsID initialization of containers (tcDesc, etc.) to use C++11 syntax and eliminate the use of a singleton initializer.
  * Update SWIG Bindings to wrap enumerations more correctly.

Fixes since v6.0.0
--------------------!
  * Fix various pieces of code to resolve warning messages on various platforms.
  * Fix OrbSysGpsL_55 Restore output of text message
  * Fix core/lib include statements to search GPSTk include directory.
  * Fix GalEphemeris Corrected behavior of isHealthy()
