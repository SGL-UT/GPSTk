GPSTk 2.12.1 Release Notes
========================

Updates since v2.12
---------------------

**Library Changes**
 * Since the existing ObsID definitions for the E5 signals predate the current Galileo ICD,  Updates to core/lib/GNSSCore/ObsID.* are needed to correctly process the cuxrrent E5a, E5b, and E5a+b signals. Reason:
The track codes need to reflect the current definitions to support processing the current signals.
 * Added a more robust and consistent implementation for handling and retrieving satellite health (via getSVHealth) information to the various satellite position/alm/ephemeris stores.  Also added a computeXvt method that works without throwing exceptions (in contrast to getXvt). Reason: Health information is needed in many use cases but up to this point was not available across all given stores and was implemented in a different way in the few stores that had it.


**Application Changes**
 * Trivial Changes made to rstats.cpp


**Test Changes**
*New/Added Tests*
 * ext/tests/geomatics/StatsFilter_T.cpp
 * core/tests/GNSSCore/Xvt_T.cpp
 * core/tests/GNSSEph/GloEphemerisStore_T.cpp
 * core/tests/GNSSEph/OrbElemStore_T.cpp
 * core/tests/GNSSEph/OrbitEphStore_T.cpp
 * core/tests/GNSSEph/RinexEphemerisStore_T.cpp
 * core/tests/GNSSEph/SP3EphemerisStore_T.cpp
 * ext/tests/GNSSEph/OrbAlmStore_T.cpp

**Truth Data Changes**
*Updated Truth Data*
 * data/test_rstats.exp
 * data/outputs/RinSum_obspath_v211.exp
 * data/outputs/RinSum_v211_kerg.exp
 * data/outputs/RinSum_v211_nklg.exp
 * data/outputs/RinSum_v302_FAA1.exp

*New Data Files*
 * data/inputs/igs/igr20354.sp3
 * data/mixed.06n


Fixes since v2.12
--------------------
* Made slight improvement to the lib/Geomatics/FDiffFilter.cpp by computing and considering the slopes.
* Tweaks also made to lib/Geomatics/SpecialFuncs.*
