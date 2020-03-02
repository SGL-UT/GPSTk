GPSTk 3.1.0 Release Notes
========================

 * This version was created in response to additional Navigation message processing capability.

Updates since v3.0.0
---------------------

**Build System and Test Suite**
  * Adjusted build.sh to better support specifying a python3 executable.

**Gitlab CI**
  * Windows CI testing was added back.
  * Added package building and testing for redhat and debian to master branch pipelines.

**Library Changes**
  * OrbAlm/OrbAlmGen updated to support direct loading of the engineering unit values. (Better support for Nav Messages)
  * NavFilter framework updated to include classes that abstract access to the storage of the subframe data.
  * Modified YumaAlamancStore and SEMAlmanacStore to inherit from OrbAlmStore (instead of GPSAlmanacStore).

Fixes since v3.0.0
--------------------
 * Fixed a bug revealed by GPS III where endValid times was calculated incorrectly in OrbElemRinex

Removed Code due to Deprecation
-------------------------------
