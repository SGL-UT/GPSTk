GPSTk 2.10.7 Release Notes
========================

Updates since v2.10.6
----------------------

**Gitlab/Github CI**
 * Travis-CI yaml script added for external Github CI autobuilds.  
 * Official deb packages are now build for master branch builds and available as artifacts.

**Library Changes*
 * Adding L1C to ObsID and QZSS to various places. REASON: Need L1C support for upcoming GPS III launch.  QZSS already has a L1C broadcast, so it makes sense to look at that as well.
 * Added method that returns a reference to PtoNMap. Used to generate csv format of the map. REASON: Want to generate a CSV form of svNumXRef, and this method is used for that.  

*New Library Classes*
 * core/lib/NavFilter/CNav2SanityFilter.*

*New/Added Tests*
 * core/tests/NavFilter/CNav2Filter_T.cpp

*Modified Test files*
 * core/tests/GNSSEph/NavID_T.cpp





