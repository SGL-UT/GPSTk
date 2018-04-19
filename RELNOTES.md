GPSTk 2.10.2 Release Notes
========================

Updates since v2.10.1
----------------------

**Build System and Test Suite**
 * Test coverage was added on the core-build using gcov and gcovr
 * Support for GootleTest/GoogleMock added.

**Gitlab CI**
 * Test coverage was added on the core-build using gcov and gcovr
 * Style compliance checking was added (as a new CI-Pipeline stage) using cpplint, but is not enforced.

**Library Changes**
 * Added code to core/lib/GNSSEph/RationalizeRinex (used by core/apps/Rinextools/scanBrdcFile) to detect RINEX nav data sets tagged with the wrong SV PRN ID and to not copy such data sets to the output file. REASON: Latest IGS-generated merged RINEX nav (brdc) files contain data sets that are tagged with the incorrect PRN ID.
 * NavFilter updates/enhancements -  Add a nav filter for forced ordering of data.  Add methods for determining filter depth for buffer size computation. ITT receivers in particular do not always output nav subframes in time order or on time or with correct time stamps.
 * Added ObservationType for demodulator status codes, which will support changes to tools in SGLTk made in a future merge
 * ORD Refactor with CI Pipeline Changes - ord.cpp was exposed in the SWIG interface. Unit tests for coverage were written using GoogleTest/GoogleMock. Test coverage was measured on the core-build using gcov and gcovr. REASON: The ORD routines was considered general enough to add to GPSTk.  The project driving it is the HRTR Web Development effort.
 * Updated references for 54/18 and 34/18 and updated PRN 4 gap
 * Added LNAVOrderFilter for forcing time-order of legacy nav subframe data. REASON: When processing data from multiple receivers, the data may not be sent by the receivers at the same time.  This filter makes a "best effort" to enforce time ordering across multiple receivers going into the filter.
* Added sp3d file format support. REASON: We care about sp3d because the IGS MGEX precise orbit files moved from sp3c to sp3d in fall 2017.  Therefore, to support various multi-GNSS data experiments, we need to be able to access these files.

*New Library Classes*
 * core/lib/ORD/ord.cpp
 * core/lib/ORD/ord.hpp
 * core/lib/NavFilter/LNavOrderFilter.cpp
 * core/lib/NavFilter/LNavOrderFilter.hpp

**Application Changes**
* core/apps/Rinextools/scanBrdcFile updated to detect RINEX nav data sets tagged with the wrong SV PRN ID and to not copy such data sets to the output file.
* Update core/apps/checktools/rowcheck.cpp & core/apps/checktools/rnwcheck.cpp to use newer RINEX classes for rowcheck and rnwcheck to make them RINEX3 compatible

*Updated Applications*
 * core/apps/Rinextools/scanBrdcFile
 * core/apps/checktools/rnwcheck.cpp
 * core/apps/checktools/rowcheck.cpp
 * core/tests/ORD/OrdRegressionChecks_T.cpp
 * core/tests/ORD/OrdUnitTests_T.cpp

**Test Changes**

*New/Added Tests*
 * core/tests/ORD/GTestExperiments.cpp
 * core/tests/ORD/OrdMockClasses.hpp

*Modified Test files*
 * core/tests/NavFilter/NavFilterMgr_T.cpp
 * ext/tests/GNSSEph/OrbAlmStore_T.cpp
 * core/tests/FileHandling/SP3_T.cpp

**Truth Data Changes**

*New Truth Data*
 * data/test_input_SP3d.sp3

**SWIG Bindings & Python** 
 * Updated SWIG interface files for ORD refactor

*New/Added SWIG Files*
 * swig/src/ord.i


Fixes since v2.10.1
-------------------
 * Corrected prettying print of eph health bits in core/lib/GNSSEph/EngEphemeris.cpp
 * Fixed incorrect end time for 54/18 in core/lib/GNSSCore/SVNumXRef.cpp
 * Fixing bug that introduced false positive on Downstream GPSTk Core build



