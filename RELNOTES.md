GPSTk 8.0.0 Release Notes
========================

 * This version addresses api change of 'PRSolution2' to 'PRSolutionLegacy' used by downstream dependency in sgltk.
 * It contains copyright updates to reflect year 2020.
 * Additionally, bug fixes and library changes were implemented.

Updates since v7.0.0
---------------------
**Build System and Test Suite**
  * Update CI to verify proper copyright and license header.
  * Update python 2.7 conda recipe to avoid using preprocessig-selector for enum34.
  
**Gitlab CI**
  * Add Centos8 build/test/package/deploy jobs to the CI pieline.
  * Update CODEOWNERS file.
  * Refactor pipeline to use git https protocol instead of ssh  
  
**Library Changes**
  * Update Nav reader code to properly assign being/end validity values for non-GPS GNSS data.
  * Refactor `PRSolution2` to `PRSolutionLegacy`.
  * Refactor calculate_ord method definition out of header file.
  * Update SVNumXRef Add SVN77/PRN14  
  * Update copyright language to reflect year 2020
  * Update IonoModel to include all modernized GPS bands. 

Fixes since v7.0.0
--------------------
  * Fix SP3EphemerisStore to properly handle correlation data from SP3c files.
  * Fix EL8 RPM Generation to avoid including build-id files.
  * Fix OrbSysGpsL_56 Correct iono parameter units