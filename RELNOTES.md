GPSTk 2.12 Release Notes
========================

Updates since v2.11
---------------------

**General Codebase Changes**
 * File Deprecation and Ext Cleanup - Lists can be found at the end of this doc
 * Updated license and copyright headers on all relevant files.


**Build System and Test Suite**
 * Added a -n flag to build.sh to allow address sanitizer to be turned off for debug builds. REASON: Address sanitizer breaks some project builds that have the gpstk as a dependency.  Some people may not want the performance hit for every debug build.

**Gitlab CI**
 * Changes to .gitlab-ci.yml to remove some coverage metrics
 * Updated debian package build stage to only happen on master branch.
 * Updating the YML file for Gitlab CI builds to add Solaris, OSX, and Windows in as Nightly-only builds.  This means that a normal continuous gitlab ci pipeline will only build for debian and redhat. REASON: While Windows, OSX, and Solaris are important and supported platforms, they consistently provide issues when ran in a continuous pipeline and are of less priority than Redhat and Debian. Currently, CDash was used for nightly builds, but migrating everything to a single gitlab platform is best.
 * Added RPM generation to CI, to trigger with master builds as does the debian package generation.  Packages are saved as artifacts.


**Library Changes**
 * Updated SVNumsXRef for new SV. Also added end date to SVN 36/PRN 4 and added start date for SVN 74/PRN 4
Added "III" Block type.
 * Added handling for GPS CNAV/CNAV-2 joint nav data such as reduced almanac and data correction packets. REASON: Need to support these messages in the data format. ALSO: This change is coupled with sgltk branch issue_240_CNAV2.   Once this is merged the sgltk master build will be broken until that branch is also merged.
 * Set the return value in OrbAlmFactory convert default case of no match. REASON: Allows checking the return value to determine if and OrbAlm instance was successfully created from PackedNavBits.
 * Updated GNSSEph classes related to LNAV to handle LNAV fit intervals correctly. REASON: launch of GPS III SV 01 revealed misunderstandings in how GPS fit intervals work. Previous code worked fine before GPS III.  GPS III is ICD-compliant, but takes advantage of options that previous SVs did not.
 * Initialized Rinex3NavHeader class variables to allow comparisons to work better with optional fields.
 * Added L1C RINEX character codes to ObsID data structure as ObsIDInitializer did not provide character code translation for the special case of L1 carrier and CD / CP / CD+CP ranging code. REASON: Writing L1C data to a RINEX file was not possible.
 * Needed additional interfaces to query the OrbSysStore in order to: 
  * Know which NavIDs are in a store. 
  * Know which GNSS systems are in a store. 
  * Obtain a list<const OrbDataSys*> of all messages in the store that correspond to a given NavID/UID, regardless of the SV that transmitted them.
  * REASON: Existing interfaces returned a list of objects for a given SV or a given time range.  Downstream applications needed a list of all objects of a given data type (e.g., UTC corrections) in the store. The interface was extended with a new method, but all existing interfaces were maintained unchanged.
  * Provided update to correctly differentiate between BeiDou D1 and D2 data, as per Issue 374.  REASON: Can't do this downstream as the differentiation is detected by data rates and lost by the time we reach VUB files except for the NavID in the header. Also made OrbElem.svXvt() virtual to allow downstream override for unusual satellite systems.
  * Fixed OrbitEphStore::findNearOrbitEph( ) to match expected selection behavior.   Also fixed such that it will only return nav data sets that have a fit interval covering the requested time.
  * Trivial updates to Geomatics library to further PRSolve development.
  * Made getInterpolationOrder const
  * Updated core/lib/Math/Matrix/MatrixFunctors.hpp to allow 1x1 matrix, also removed unused variable
  * Add toggle bool to CommandLine.*, also count()
  * Geomatics Updates:
   * SRI: make retriangularize its own function, add a few convenience routines, and add to doc
   * SRIMatrix: Better Cholesky, add LDL and UDU
   * SatPass: add a single frequency version of smooth()
   * KalmanFilter: upgrade test
   * StatsFilter: split into two, and add a better first difference filter FDiffFilter, upgrade test
  * RINEX obs file loader - Add a class which is used to "load" i.e. read and summarize content, of a Rinex obs file (2 or 3). This is used to 'pre-read' the file to prevent stumbling because of incorrect/missing header information. Add tests of RINEX 3.03, 2.11 and 2.10 obs files. REASON: Used in PPP and differential postitioning
  * Orb Sys Store Updates - added bool function hasSignal to find out which signal the sv is using
  * Updating SVNumXRef.  PRN 4 moved from SVN 74 to SVN 36.
  * Added generalized ability to find UTCOffset

*New Library Classes*
 * ext/lib/GNSSEph/CNavMidiAlm.*
 * ext/lib/GNSSEph/CNavReducedAlm.*
 * ext/lib/GNSSEph/DiffCorrBase.*
 * ext/lib/GNSSEph/DiffCorrClk.* 
 * ext/lib/GNSSEph/DiffCorrEph.* 
 * ext/lib/Geomatics/MostCommonValue.hpp
 * ext/lib/Geomatics/Rinex3ObsFileLoader.cpp
 * ext/lib/Geomatics/WNJfilter.hpp


**Application Changes**
 * Added simpler interface to RAIMCompute, RAIMComputeSimple, by deleting the two GPSTk classes/types that aren't exposed to Python via swig: the Matrix class and a vector of SatID::SatelliteSystem enums.  RAIMComputeSimple simply declares instances of both types and then calls RAIMCompute with everything.  PRSolution builds both objects if they aren't supplied, so the code already could handle not being supplied this information. REASON: Per MDH Tools Tech Jam on 07 Jan 2019, this is the agreed temporary solution to making PRSolution accessible to PySGLTk.  The simpler interface may be deprecated upon swig modification, if deemed appropriate.
 * 1. Changed all of the Rinex usage to Rinex3 within mergeRinNav.cpp & 2. Changed the .exp files used by mergeRinNav.cpp to expect one less significant figure in the header. REASON: Changes for (1) is needed so that the result would be written in scientific notation instead of D notation as per requested in MSN-5790.  Changes for (2) is needed because increasing the amount of significant figures that Rinex3 headers writes out ensures that the new version of mergeRinNav passes its application tests. However, it causes issues in other tests. Changing to the expected files allows mergeRinNav to pass its test and other tests to pass. Also, the creation of the new mergeRinNav_1.exp and mergeRinNav_2.exp use the same input.
 * Refactored rstats.cpp and added tests. REASON: rstats evolved from a small tool to compute robust stats into a much larger toolset, a mess.
 * dfix is a new implementation of cycleslip detection and fixing, with tests.

*Updated Applications*
 * core/apps/positioning/PRSolve.cpp
 * core/apps/mergetools/mergeRinNav.cpp
 * ext/apps/geomatics/robust/rstats.cpp
 

**SWIG Bindings & Python** 
 * A hard-coded value in FileIO.i is changed so that non-GPS RINEX data can be written with the RINEX3 classes
 * Disabled rpath for swig python binaries by forcing CMake to not include RPATH arguments when building SWIG binaries.  REASON: This integrates better with our isolated python environments. (e.g. conda)
 * Add sdist python target for standalone installation by: 1) Updating the CMake files to produce an "wheel" package for python installation that contains the libgstk.so in addition to the swig'ed library and 2) Modified the init.py script to load the local libgpstk.so before importing _gpstk, bypassing the ld library search. REASON: This should allows preparing a distributable tar ball that can be installed into a python environment, that contains a pre-compiled libgpstk.so library.
 * Python CI changes & package build fix - 1) Added an additional check to ensure python is installed correctly after install step completes. 2) Changed the CMake python installation command to use --root instead of --prefix.  This is more friendly to the debian package builder, and doesn't appear to break the conda build/install process. 3) Made gpstk import warnings more verbose.
 * Added CMAKE flag to install python differently when preparing a package. REASON: Using the --root argument works for a debian package but causes problems with a "normal" installation.


**Test Changes**

*New/Added Tests*
 * ext/tests/GNSSEph/CNavPackets_T.cpp
 * core/tests/GNSSEph/OrbElemLNav_valid_T.cpp
 * ext/tests/GNSSEph/OrbSysStore_T.cpp
 * core/tests/FileDirProc/FileHunter_T.cpp
 * ext/apps/geomatics/RinexObsLoader_test/Rinex3ObsLoader_T.cpp
 * ext/apps/geomatics/rstats_test/SDexam01.txt
 * ext/apps/geomatics/rstats_test/rstats_T.py


**Truth Data Changes**

*Updated Truth Data*
 * data/outputs/RinSum_obspath_v211.exp
 * data/outputs/RinSum_v210_osn3.exp
 * data/outputs/RinSum_v211_cags.exp
 * data/outputs/RinSum_v211_kerg.exp
 * data/outputs/RinSum_v211_nklg.exp
 * data/outputs/RinSum_v302_FAA1.exp
 * data/PRSolve_Required.exp

*Modified Truth Data*
 * data/mergeRinNav_1.exp
 * data/mergeRinNav_2.exp
 * data/test_output_OrbDataSys_T_GPS_CNAV.exp
 * data/test_output_OrbDataSys_T_GPS_LNAV.exp

*New Data Files*
 * data/Rinex3ObsLoader210.obs
 * data/Rinex3ObsLoader211.obs
 * data/Rinex3ObsLoader303.obs
 * data/Rinex3ObsLoader_R210.obs
 * data/Rinex3ObsLoader_R211.obs
 * data/Rinex3ObsLoader_R303.obs
 * data/test_rstats.exp
 * ext/apps/geomatics/rstats_test/testfft.data
 * data/test_dfix_job4131.ed.obs
 * data/test_dfix_job4131.ed.obs
 * data/test_dfix_job4131.exp
 * data/test_dfix_karr.exp
 * data/test_dfix_karr0880.ed.10o
 * data/test_dfix_tower.exp
 * data/test_dfix_tower239.ed.15o
 * data/test_dfix_txau.exp
 * data/test_dfix_txau047.ed.12o


Fixes since v2.11
--------------------
* RinSum now prints the Marker name as part of the output.  PRSolve also no longer prints an extra line in the output on Windows. It was a matter of using LOGstrm instead of LOG(INFO) in the timing output. REASON: See issues #355 & #356
 * Modified the swig build to include the gpstk version as a top-level variable in the python module as installed tools could not determine version.
 * Fixed FileHunter::find() time system bug and added relevant unit tests.
 * Fixed Issue 375 (erroneously treating files as directories) and mostly addressed Issue 229 (added progressive time filtering for better performance).  Also performed some general code cleanup and improved exception handling and doxygen.
 * Fixed 8-bit week rollover error in ext/lib/GNSSEph/OrbSysGpsL_51.cpp. REASON: 127 is not 28, 256 is 28
 * Fixed Windows compiler errors that were preventing building on Windows.
 * Fixed bug in RobustStats:: stem-and-leaf plot
 * Fixed debian package creation for gpstk-python. REASON: Adding the python wheel build option in advertently broke the debian package creation for gpstk-python.

Code Staged for Future Deprecation
-----------------------------------
* /apps
  * /clocktools
    *  rmoutlier
    *  dallandev
    *  ffp
    *  mallandev
    *  nallandev
    *  oallandev
    *  ohadamarddev
    *  ORDPhaseParser
    *  pff
    *  scale
    *  tallandev
    *  TIAPhaseParser
    *  trunc
    *  allanplot.py
  * /differential
    *  vecsol
  * /filetools
    *  GloDump
    *  rinexthin
  * /positioning
    *  posInterp
* /lib
  * /Geodyn
    *  ASConstant.hpp
    *  AtmosphericDrag
    *  CiraExpotentialDrag
    *  EarthBody
    *  EarthOceanTide
    *  EarthPoleTide
    *  EarthSolidTide
    *  EGM96GravityModel
    *  EquationOfMotion.hpp
    *  ForceModel.hpp
    *  ForceModelList
    *  HarrisPriesterDrag
    *  IERS
    *  IERSConventions
    *  Integrator.hpp
    *  JGM3GravityModel
    *  KeplerOrbit
    *  MoonForce
    *  Msise00Drag
    *  PvtStore
    *  ReferenceFrames
    *  RelativityEffect
    *  RungeKuttaFehlberg
    *  SatOrbit
    *  SatOrbitPropogator
    *  SolarRadiationPressure
    *  Spacecraft
    *  SphericalHarmonicGravity
    *  SunForce
    *  UTCTime

Removed Code due to Deprecation
-------------------------------
 * Deprecated code that was in ext and causing build failures in debian stretch.
   * changed auto_ptr to unique_ptr
   * added -std=c++11 flag to linux build (sorry solaris)
   * replaced google-mock with googletest in SGL_BASE in fai for stretch
   * made google mock look in /usr/src/googletest/googlemock for gmock
   * added /deprecated dir for deprecated code
   * deprecated several libs/apps in ext that were causing some failures due to c++ deprecations.
 * /apps/multipath/CMakeLists.txt
 * /apps/multipath/DenseBinnedStats.hpp
 * /apps/multipath/ObsArray.*
 * /apps/multipath/SparseBinnedStats.hpp
 * /apps/multipath/ValarrayUtils.hpp
 * /apps/multipath/mpsim.cpp
 * /apps/multipath/mpsolve.cpp
 * /apps/multipath/testObsArray.cpp
 * /apps/multipath/testSparseBinnedStats.cpp
 * /apps/multipath/testValarrayUtils.cpp
 * /apps/receiver/AshtechMessage.*
 * /apps/receiver/CMakeLists.txt
 * /apps/receiver/ScreenControl.*
 * /apps/receiver/TODO.txt
 * /apps/receiver/getUnixSerialInfo.cpp
 * /apps/receiver/rinex.nav.template
 * /apps/receiver/rinex.obs.template
 * /apps/receiver/rtAshtech.cpp
 * /apps/swrx/CACodeGenerator.hpp
 * /apps/swrx/CCReplica.*
 * /apps/swrx/CMakeLists.txt
 * /apps/swrx/CodeGenerator.hpp
 * /apps/swrx/ConstLinearRecurrentSequence.*
 * /apps/swrx/EMLTracker.*
 * /apps/swrx/IQStream.*
 * /apps/swrx/NAVCodeGenerator.hpp
 * /apps/swrx/NavFramer.*
 * /apps/swrx/P0CodeGenerator.*
 * /apps/swrx/RX.cpp
 * /apps/swrx/SVSource.hpp
 * /apps/swrx/SimpleCorrelator.hpp
 * /apps/swrx/acquire.cpp
 * /apps/swrx/codeDump.cpp
 * /apps/swrx/complex_math.h
 * /apps/swrx/corltr.cpp
 * /apps/swrx/gpsSim.cpp
 * /apps/swrx/hilbert.cpp
 * /apps/swrx/iqdump.cpp
 * /apps/swrx/normal.*
 * /apps/swrx/plot
 * /apps/swrx/position.cpp
 * /apps/swrx/simpleNav.cpp
 * /apps/swrx/tracker.cpp
 * /apps/swrx/trackerMT.cpp
 * /apps/time/CMakeLists.txt
 * /apps/time/calgps_svg.cpp
 * /apps/time/generateCalendars.bash
 * /examples/example10.cpp
 * /examples/example11.cpp
 * /examples/example13.cpp
 * /examples/example14.cpp
 * /examples/example16.cpp
 * /examples/example17.cpp
 * /examples/example18.cpp
 * /examples/example5.cpp
 * /examples/example6.cpp
 * /examples/example7.cpp
 * /examples/example8.cpp
 * /examples/example9.cpp
 * /lib/CNAV2EphClk.*
 * /lib/CNAVClock.*
 * /lib/CNAVEphemeris.*
 * /lib/Math/RungeKutta4.*
 * /lib/Math/SimpleKalmanFilter.*
 * /lib/Procframe/Antenna.*
 * /lib/Procframe/AntexReader.*
 * /lib/Procframe/BasicModel.*
 * /lib/Procframe/CheckPRData.hpp
 * /lib/Procframe/CodeKalmanSolver.*
 * /lib/Procframe/CodeSmoother.*
 * /lib/Procframe/ComputeCombination.*
 * /lib/Procframe/ComputeDOP.*
 * /lib/Procframe/ComputeIURAWeights.*
 * /lib/Procframe/ComputeIonoModel.*
 * /lib/Procframe/ComputeLC.*
 * /lib/Procframe/ComputeLI.*
 * /lib/Procframe/ComputeLdelta.*
 * /lib/Procframe/ComputeLinear.*
 * /lib/Procframe/ComputeMOPSWeights.*
 * /lib/Procframe/ComputeMelbourneWubbena.*
 * /lib/Procframe/ComputePC.*
 * /lib/Procframe/ComputePI.*
 * /lib/Procframe/ComputePdelta.*
 * /lib/Procframe/ComputeSatPCenter.*
 * /lib/Procframe/ComputeSimpleWeights.*
 * /lib/Procframe/ComputeTropModel.*
 * /lib/Procframe/ComputeWindUp.*
 * /lib/Procframe/ConfData.hpp
 * /lib/Procframe/ConfDataItem.hpp
 * /lib/Procframe/ConfDataSection.hpp
 * /lib/Procframe/ConfDataStructures.hpp
 * /lib/Procframe/ConstraintSystem.*
 * /lib/Procframe/ConvertC1ToP1.*
 * /lib/Procframe/CorrectCodeBiases.*
 * /lib/Procframe/CorrectObservables.*
 * /lib/Procframe/DataHeaders.*
 * /lib/Procframe/DataStructures.*
 * /lib/Procframe/Decimate.*
 * /lib/Procframe/DeltaOp.*
 * /lib/Procframe/Differentiator.*
 * /lib/Procframe/DoubleOp.*
 * /lib/Procframe/Dumper.*
 * /lib/Procframe/EclipsedSatFilter.*
 * /lib/Procframe/Equation.*
 * /lib/Procframe/EquationSystem.*
 * /lib/Procframe/ExtractCombinationData.*
 * /lib/Procframe/ExtractData.*
 * /lib/Procframe/ExtractLC.hpp
 * /lib/Procframe/ExtractPC.hpp
 * /lib/Procframe/GDSUtils.hpp
 * /lib/Procframe/GeneralConstraint.*
 * /lib/Procframe/GeneralEquations.*
 * /lib/Procframe/GravitationalDelay.*
 * /lib/Procframe/IonexModel.*
 * /lib/Procframe/Keeper.*
 * /lib/Procframe/LICSDetector.*
 * /lib/Procframe/LICSDetector2.*
 * /lib/Procframe/LinearCombinations.*
 * /lib/Procframe/MOPSWeight.*
 * /lib/Procframe/MWCSDetector.*
 * /lib/Procframe/MemoryUtils.hpp
 * /lib/Procframe/ModelObs.*
 * /lib/Procframe/ModelObsFixedStation.*
 * /lib/Procframe/ModeledPR.*
 * /lib/Procframe/ModeledPseudorangeBase.hpp
 * /lib/Procframe/ModeledReferencePR.*
 * /lib/Procframe/NablaOp.*
 * /lib/Procframe/NetworkObsStreams.*
 * /lib/Procframe/OneFreqCSDetector.*
 * /lib/Procframe/PCSmoother.*
 * /lib/Procframe/PhaseCodeAlignment.*
 * /lib/Procframe/ProblemSatFilter.*
 * /lib/Procframe/ProcessingClass.hpp
 * /lib/Procframe/ProcessingList.*
 * /lib/Procframe/ProcessingVector.*
 * /lib/Procframe/Pruner.*
 * /lib/Procframe/RequireObservables.*
 * /lib/Procframe/SatArcMarker.*
 * /lib/Procframe/SimpleFilter.*
 * /lib/Procframe/SimpleIURAWeight.*
 * /lib/Procframe/SolverBase.hpp
 * /lib/Procframe/SolverGeneral.*
 * /lib/Procframe/SolverLMS.*
 * /lib/Procframe/SolverPPP.*
 * /lib/Procframe/SolverPPPFB.*
 * /lib/Procframe/SolverWMS.*
 * /lib/Procframe/SourceID.*
 * /lib/Procframe/StochasticModel.*
 * /lib/Procframe/Synchronize.*
 * /lib/Procframe/TypeID.*
 * /lib/Procframe/Variable.*
 * /lib/Procframe/WeightBase.hpp
 * /lib/Procframe/XYZ2NED.*
 * /lib/Procframe/XYZ2NEU.*
 * /lib/SVExclusionList.*
 * /lib/Vdraw/Adler32.*
 * /lib/Vdraw/Base64Encoder.*
 * /lib/Vdraw/BasicShape.hpp
 * /lib/Vdraw/Bitmap.*
 * /lib/Vdraw/BorderLayout.*
 * /lib/Vdraw/CRC32.*
 * /lib/Vdraw/Canvas.*
 * /lib/Vdraw/Circle.hpp
 * /lib/Vdraw/Color.*
 * /lib/Vdraw/ColorMap.*
 * /lib/Vdraw/Comment.*
 * /lib/Vdraw/EPSImage.*
 * /lib/Vdraw/Fillable.hpp
 * /lib/Vdraw/Frame.*
 * /lib/Vdraw/GraphicsConstants.*
 * /lib/Vdraw/GridLayout.*
 * /lib/Vdraw/HLayout.*
 * /lib/Vdraw/Helper.hpp
 * /lib/Vdraw/InterpolatedColorMap.*
 * /lib/Vdraw/Layout.hpp
 * /lib/Vdraw/Line.*
 * /lib/Vdraw/Markable.hpp
 * /lib/Vdraw/Marker.*
 * /lib/Vdraw/PNG.*
 * /lib/Vdraw/PSImage.*
 * /lib/Vdraw/PSImageBase.*
 * /lib/Vdraw/Palette.*
 * /lib/Vdraw/Path.*
 * /lib/Vdraw/Polygon.hpp
 * /lib/Vdraw/Rectangle.*
 * /lib/Vdraw/SVGImage.*
 * /lib/Vdraw/StrokeStyle.*
 * /lib/Vdraw/Text.*
 * /lib/Vdraw/TextStyle.*
 * /lib/Vdraw/VDrawException.hpp
 * /lib/Vdraw/VGImage.*
 * /lib/Vdraw/VGState.hpp
 * /lib/Vdraw/VLayout.*
 * /lib/Vdraw/ViewerManager.*
 * /lib/Vplot/Axis.*
 * /lib/Vplot/AxisStyle.hpp
 * /lib/Vplot/LinePlot.*
 * /lib/Vplot/Plot.*
 * /lib/Vplot/ScatterPlot.*
 * /lib/Vplot/SeriesList.*
 * /lib/Vplot/Splitter.*
 * /lib/Vplot/SurfacePlot.*
 * /lib/Vplot/plottypes.hpp
 * /lib/Xv.hpp
 * /lib/compass_constants.hpp
 * /lib/geometry.hpp
 * /tests/multipath/CMakeLists.txt
 * /tests/multipath/testmpsolve.cmake
 * /tests/oldtests/AnotherFileFilterTest.cpp
 * /tests/oldtests/EphComp.cpp
 * /tests/oldtests/EphCompWin.gp
 * /tests/oldtests/FileSpecTest.cpp
 * /tests/oldtests/Jamfile
 * /tests/oldtests/Makefile.am
 * /tests/oldtests/MinSfTest.cpp
 * /tests/oldtests/Rinex_dl.pl
 * /tests/oldtests/RungeKuttaTest.cpp
 * /tests/oldtests/Xbegweek.cpp
 * /tests/oldtests/Xendweek.cpp
 * /tests/oldtests/configfile.txt
 * /tests/oldtests/configfile_readme.txt
 * /tests/oldtests/data/405_077A.02M
 * /tests/oldtests/data/MatrixTest.ref
 * /tests/oldtests/data/MatrixTest.ref.Win32
 * /tests/oldtests/data/Xbegweek.can
 * /tests/oldtests/data/Xendweek.can
 * /tests/oldtests/data/anotsym.dat
 * /tests/oldtests/data/cov.dat
 * /tests/oldtests/data/dia.dat
 * /tests/oldtests/data/lt.dat
 * /tests/oldtests/data/nga12600.apc
 * /tests/oldtests/data/nga12601.apc
 * /tests/oldtests/data/partials.dat
 * /tests/oldtests/data/positiontest.ref
 * /tests/oldtests/data/squ.dat
 * /tests/oldtests/data/stringutiltest.ref
 * /tests/oldtests/data/sym.dat
 * /tests/oldtests/data/tmatrix.dat
 * /tests/oldtests/data/ut.dat
 * /tests/oldtests/delFileSpecTestDirs.pl
 * /tests/oldtests/exceptiontest.cpp
 * /tests/oldtests/genFileSpecTestDirs.pl
 * /tests/oldtests/petest.cpp
 * /tests/oldtests/rinex_met_livetest.pl
 * /tests/oldtests/runAllTests
 * /tests/oldtests/runAllTests.bat
 * /tests/oldtests/svnKeyWordTest.txt
 * /tests/oldtests/testExpression.cpp
 * /tests/oldtests/testscript.pl
 * /tests/oldtests/testscript_readme.txt
 * /tests/time/CMakeLists.txt