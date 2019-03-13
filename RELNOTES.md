GPSTk 2.11.1 Release Notes
========================

Updates since v2.11
------------------

**Gitlab CI**
 * Changes to .gitlab-ci.yml to remove some coverage metrics
 * Updated debian package build stage to only happen on master branch.


**Ext -> Deprecate Migration**
 * Depricated code that was in ext and causing build failures in debian stretch.
   * changed auto_ptr to unique_ptr
   * added -std=c++11 flag to linux build (sorry solaris)
   * replaced google-mock with googletest in SGL_BASE in fai for stretch
   * made google mock look in /usr/src/googletest/googlemock for gmock
   * added /deprecated dir for deprecated code
   * deprecated several libs/apps in ext that were causing some failures due to c++ deprecations.
 * depricated/apps/multipath/CMakeLists.txt
 * depricated/apps/multipath/DenseBinnedStats.hpp
 * depricated/apps/multipath/ObsArray.*
 * depricated/apps/multipath/SparseBinnedStats.hpp
 * depricated/apps/multipath/ValarrayUtils.hpp
 * depricated/apps/multipath/mpsim.cpp
 * depricated/apps/multipath/mpsolve.cpp
 * depricated/apps/multipath/testObsArray.cpp
 * depricated/apps/multipath/testSparseBinnedStats.cpp
 * depricated/apps/multipath/testValarrayUtils.cpp
 * depricated/apps/receiver/AshtechMessage.*
 * depricated/apps/receiver/CMakeLists.txt
 * depricated/apps/receiver/ScreenControl.*
 * depricated/apps/receiver/TODO.txt
 * depricated/apps/receiver/getUnixSerialInfo.cpp
 * depricated/apps/receiver/rinex.nav.template
 * depricated/apps/receiver/rinex.obs.template
 * depricated/apps/receiver/rtAshtech.cpp
 * depricated/apps/swrx/CACodeGenerator.hpp
 * depricated/apps/swrx/CCReplica.*
 * depricated/apps/swrx/CMakeLists.txt
 * depricated/apps/swrx/CodeGenerator.hpp
 * depricated/apps/swrx/ConstLinearRecurrentSequence.*
 * depricated/apps/swrx/EMLTracker.*
 * depricated/apps/swrx/IQStream.*
 * depricated/apps/swrx/NAVCodeGenerator.hpp
 * depricated/apps/swrx/NavFramer.*
 * depricated/apps/swrx/P0CodeGenerator.*
 * depricated/apps/swrx/RX.cpp
 * depricated/apps/swrx/SVSource.hpp
 * depricated/apps/swrx/SimpleCorrelator.hpp
 * depricated/apps/swrx/acquire.cpp
 * depricated/apps/swrx/codeDump.cpp
 * depricated/apps/swrx/complex_math.h
 * depricated/apps/swrx/corltr.cpp
 * depricated/apps/swrx/gpsSim.cpp
 * depricated/apps/swrx/hilbert.cpp
 * depricated/apps/swrx/iqdump.cpp
 * depricated/apps/swrx/normal.*
 * depricated/apps/swrx/plot
 * depricated/apps/swrx/position.cpp
 * depricated/apps/swrx/simpleNav.cpp
 * depricated/apps/swrx/tracker.cpp
 * depricated/apps/swrx/trackerMT.cpp
 * depricated/apps/time/CMakeLists.txt
 * depricated/apps/time/calgps_svg.cpp
 * depricated/apps/time/generateCalendars.bash
 * depricated/examples/example10.cpp
 * depricated/examples/example11.cpp
 * depricated/examples/example13.cpp
 * depricated/examples/example14.cpp
 * depricated/examples/example16.cpp
 * depricated/examples/example17.cpp
 * depricated/examples/example18.cpp
 * depricated/examples/example5.cpp
 * depricated/examples/example6.cpp
 * depricated/examples/example7.cpp
 * depricated/examples/example8.cpp
 * depricated/examples/example9.cpp
 * depricated/lib/CNAV2EphClk.*
 * depricated/lib/CNAVClock.*
 * depricated/lib/CNAVEphemeris.*
 * depricated/lib/Math/RungeKutta4.*
 * depricated/lib/Math/SimpleKalmanFilter.*
 * depricated/lib/Procframe/Antenna.*
 * depricated/lib/Procframe/AntexReader.*
 * depricated/lib/Procframe/BasicModel.*
 * depricated/lib/Procframe/CheckPRData.hpp
 * depricated/lib/Procframe/CodeKalmanSolver.*
 * depricated/lib/Procframe/CodeSmoother.*
 * depricated/lib/Procframe/ComputeCombination.*
 * depricated/lib/Procframe/ComputeDOP.*
 * depricated/lib/Procframe/ComputeIURAWeights.*
 * depricated/lib/Procframe/ComputeIonoModel.*
 * depricated/lib/Procframe/ComputeLC.*
 * depricated/lib/Procframe/ComputeLI.*
 * depricated/lib/Procframe/ComputeLdelta.*
 * depricated/lib/Procframe/ComputeLinear.*
 * depricated/lib/Procframe/ComputeMOPSWeights.*
 * depricated/lib/Procframe/ComputeMelbourneWubbena.*
 * depricated/lib/Procframe/ComputePC.*
 * depricated/lib/Procframe/ComputePI.*
 * depricated/lib/Procframe/ComputePdelta.*
 * depricated/lib/Procframe/ComputeSatPCenter.*
 * depricated/lib/Procframe/ComputeSimpleWeights.*
 * depricated/lib/Procframe/ComputeTropModel.*
 * depricated/lib/Procframe/ComputeWindUp.*
 * depricated/lib/Procframe/ConfData.hpp
 * depricated/lib/Procframe/ConfDataItem.hpp
 * depricated/lib/Procframe/ConfDataSection.hpp
 * depricated/lib/Procframe/ConfDataStructures.hpp
 * depricated/lib/Procframe/ConstraintSystem.*
 * depricated/lib/Procframe/ConvertC1ToP1.*
 * depricated/lib/Procframe/CorrectCodeBiases.*
 * depricated/lib/Procframe/CorrectObservables.*
 * depricated/lib/Procframe/DataHeaders.*
 * depricated/lib/Procframe/DataStructures.*
 * depricated/lib/Procframe/Decimate.*
 * depricated/lib/Procframe/DeltaOp.*
 * depricated/lib/Procframe/Differentiator.*
 * depricated/lib/Procframe/DoubleOp.*
 * depricated/lib/Procframe/Dumper.*
 * depricated/lib/Procframe/EclipsedSatFilter.*
 * depricated/lib/Procframe/Equation.*
 * depricated/lib/Procframe/EquationSystem.*
 * depricated/lib/Procframe/ExtractCombinationData.*
 * depricated/lib/Procframe/ExtractData.*
 * depricated/lib/Procframe/ExtractLC.hpp
 * depricated/lib/Procframe/ExtractPC.hpp
 * depricated/lib/Procframe/GDSUtils.hpp
 * depricated/lib/Procframe/GeneralConstraint.*
 * depricated/lib/Procframe/GeneralEquations.*
 * depricated/lib/Procframe/GravitationalDelay.*
 * depricated/lib/Procframe/IonexModel.*
 * depricated/lib/Procframe/Keeper.*
 * depricated/lib/Procframe/LICSDetector.*
 * depricated/lib/Procframe/LICSDetector2.*
 * depricated/lib/Procframe/LinearCombinations.*
 * depricated/lib/Procframe/MOPSWeight.*
 * depricated/lib/Procframe/MWCSDetector.*
 * depricated/lib/Procframe/MemoryUtils.hpp
 * depricated/lib/Procframe/ModelObs.*
 * depricated/lib/Procframe/ModelObsFixedStation.*
 * depricated/lib/Procframe/ModeledPR.*
 * depricated/lib/Procframe/ModeledPseudorangeBase.hpp
 * depricated/lib/Procframe/ModeledReferencePR.*
 * depricated/lib/Procframe/NablaOp.*
 * depricated/lib/Procframe/NetworkObsStreams.*
 * depricated/lib/Procframe/OneFreqCSDetector.*
 * depricated/lib/Procframe/PCSmoother.*
 * depricated/lib/Procframe/PhaseCodeAlignment.*
 * depricated/lib/Procframe/ProblemSatFilter.*
 * depricated/lib/Procframe/ProcessingClass.hpp
 * depricated/lib/Procframe/ProcessingList.*
 * depricated/lib/Procframe/ProcessingVector.*
 * depricated/lib/Procframe/Pruner.*
 * depricated/lib/Procframe/RequireObservables.*
 * depricated/lib/Procframe/SatArcMarker.*
 * depricated/lib/Procframe/SimpleFilter.*
 * depricated/lib/Procframe/SimpleIURAWeight.*
 * depricated/lib/Procframe/SolverBase.hpp
 * depricated/lib/Procframe/SolverGeneral.*
 * depricated/lib/Procframe/SolverLMS.*
 * depricated/lib/Procframe/SolverPPP.*
 * depricated/lib/Procframe/SolverPPPFB.*
 * depricated/lib/Procframe/SolverWMS.*
 * depricated/lib/Procframe/SourceID.*
 * depricated/lib/Procframe/StochasticModel.*
 * depricated/lib/Procframe/Synchronize.*
 * depricated/lib/Procframe/TypeID.*
 * depricated/lib/Procframe/Variable.*
 * depricated/lib/Procframe/WeightBase.hpp
 * depricated/lib/Procframe/XYZ2NED.*
 * depricated/lib/Procframe/XYZ2NEU.*
 * depricated/lib/SVExclusionList.*
 * depricated/lib/Vdraw/Adler32.*
 * depricated/lib/Vdraw/Base64Encoder.*
 * depricated/lib/Vdraw/BasicShape.hpp
 * depricated/lib/Vdraw/Bitmap.*
 * depricated/lib/Vdraw/BorderLayout.*
 * depricated/lib/Vdraw/CRC32.*
 * depricated/lib/Vdraw/Canvas.*
 * depricated/lib/Vdraw/Circle.hpp
 * depricated/lib/Vdraw/Color.*
 * depricated/lib/Vdraw/ColorMap.*
 * depricated/lib/Vdraw/Comment.*
 * depricated/lib/Vdraw/EPSImage.*
 * depricated/lib/Vdraw/Fillable.hpp
 * depricated/lib/Vdraw/Frame.*
 * depricated/lib/Vdraw/GraphicsConstants.*
 * depricated/lib/Vdraw/GridLayout.*
 * depricated/lib/Vdraw/HLayout.*
 * depricated/lib/Vdraw/Helper.hpp
 * depricated/lib/Vdraw/InterpolatedColorMap.*
 * depricated/lib/Vdraw/Layout.hpp
 * depricated/lib/Vdraw/Line.*
 * depricated/lib/Vdraw/Markable.hpp
 * depricated/lib/Vdraw/Marker.*
 * depricated/lib/Vdraw/PNG.*
 * depricated/lib/Vdraw/PSImage.*
 * depricated/lib/Vdraw/PSImageBase.*
 * depricated/lib/Vdraw/Palette.*
 * depricated/lib/Vdraw/Path.*
 * depricated/lib/Vdraw/Polygon.hpp
 * depricated/lib/Vdraw/Rectangle.*
 * depricated/lib/Vdraw/SVGImage.*
 * depricated/lib/Vdraw/StrokeStyle.*
 * depricated/lib/Vdraw/Text.*
 * depricated/lib/Vdraw/TextStyle.*
 * depricated/lib/Vdraw/VDrawException.hpp
 * depricated/lib/Vdraw/VGImage.*
 * depricated/lib/Vdraw/VGState.hpp
 * depricated/lib/Vdraw/VLayout.*
 * depricated/lib/Vdraw/ViewerManager.*
 * depricated/lib/Vplot/Axis.*
 * depricated/lib/Vplot/AxisStyle.hpp
 * depricated/lib/Vplot/LinePlot.*
 * depricated/lib/Vplot/Plot.*
 * depricated/lib/Vplot/ScatterPlot.*
 * depricated/lib/Vplot/SeriesList.*
 * depricated/lib/Vplot/Splitter.*
 * depricated/lib/Vplot/SurfacePlot.*
 * depricated/lib/Vplot/plottypes.hpp
 * depricated/lib/Xv.hpp
 * depricated/lib/compass_constants.hpp
 * depricated/lib/geometry.hpp
 * depricated/tests/multipath/CMakeLists.txt
 * depricated/tests/multipath/testmpsolve.cmake
 * depricated/tests/oldtests/AnotherFileFilterTest.cpp
 * depricated/tests/oldtests/EphComp.cpp
 * depricated/tests/oldtests/EphCompWin.gp
 * depricated/tests/oldtests/FileSpecTest.cpp
 * depricated/tests/oldtests/Jamfile
 * depricated/tests/oldtests/Makefile.am
 * depricated/tests/oldtests/MinSfTest.cpp
 * depricated/tests/oldtests/Rinex_dl.pl
 * depricated/tests/oldtests/RungeKuttaTest.cpp
 * depricated/tests/oldtests/Xbegweek.cpp
 * depricated/tests/oldtests/Xendweek.cpp
 * depricated/tests/oldtests/configfile.txt
 * depricated/tests/oldtests/configfile_readme.txt
 * depricated/tests/oldtests/data/405_077A.02M
 * depricated/tests/oldtests/data/MatrixTest.ref
 * depricated/tests/oldtests/data/MatrixTest.ref.Win32
 * depricated/tests/oldtests/data/Xbegweek.can
 * depricated/tests/oldtests/data/Xendweek.can
 * depricated/tests/oldtests/data/anotsym.dat
 * depricated/tests/oldtests/data/cov.dat
 * depricated/tests/oldtests/data/dia.dat
 * depricated/tests/oldtests/data/lt.dat
 * depricated/tests/oldtests/data/nga12600.apc
 * depricated/tests/oldtests/data/nga12601.apc
 * depricated/tests/oldtests/data/partials.dat
 * depricated/tests/oldtests/data/positiontest.ref
 * depricated/tests/oldtests/data/squ.dat
 * depricated/tests/oldtests/data/stringutiltest.ref
 * depricated/tests/oldtests/data/sym.dat
 * depricated/tests/oldtests/data/tmatrix.dat
 * depricated/tests/oldtests/data/ut.dat
 * depricated/tests/oldtests/delFileSpecTestDirs.pl
 * depricated/tests/oldtests/exceptiontest.cpp
 * depricated/tests/oldtests/genFileSpecTestDirs.pl
 * depricated/tests/oldtests/petest.cpp
 * depricated/tests/oldtests/rinex_met_livetest.pl
 * depricated/tests/oldtests/runAllTests
 * depricated/tests/oldtests/runAllTests.bat
 * depricated/tests/oldtests/svnKeyWordTest.txt
 * depricated/tests/oldtests/testExpression.cpp
 * depricated/tests/oldtests/testscript.pl
 * depricated/tests/oldtests/testscript_readme.txt
 * depricated/tests/time/CMakeLists.txt


**Library Changes**
 * Updated SVNumsXRef for new SV. Also added end date to SVN 36/PRN 4 and added start date for SVN 74/PRN 4
Added "III" Block type.
 * Added handling for GPS CNAV/CNAV-2 joint nav data such as reduced almanac and data correction packets. REASON: Need to support these messages in the data format. ALSO: This change is coupled with sgltk branch issue_240_CNAV2.   Once this is merged the sgltk master build will be broken until that branch is also merged.
 * Set the return value in OrbAlmFactory convert default case of no match. REASON: Allows checking the return value to determine if and OrbAlm instance was successfully created from PackedNavBits.
 * Updated GNSSEph classes related to LNAV to handle LNAV fit intervals correctly. REASON: launch of GPS III SV 01 revealed misunderstandings in how GPS fit intervals work. Previous code worked fine before GPS III.  GPS III is ICD-compliant, but takes advantage of options that previous SVs did not.

*New Library Classes*
 * ext/lib/GNSSEph/CNavMidiAlm.*
 * ext/lib/GNSSEph/CNavReducedAlm.*
 * ext/lib/GNSSEph/DiffCorrBase.*
 * ext/lib/GNSSEph/DiffCorrClk.* 
 * ext/lib/GNSSEph/DiffCorrEph.* 


**Application Changes**
 * Added simpler interface to RAIMCompute, RAIMComputeSimple, by deleting the two GPSTk classes/types that aren't exposed to Python via swig: the Matrix class and a vector of SatID::SatelliteSystem enums.  RAIMComputeSimple simply declares instances of both types and then calls RAIMCompute with everything.  PRSolution builds both objects if they aren't supplied, so the code already could handle not being supplied this information. REASON: Per MDH Tools Tech Jam on 07 Jan 2019, this is the agreed temporary solution to making PRSolution accessible to PySGLTk.  The simpler interface may be deprecated upon swig modification, if deemed appropriate.

*Updated Applications*
 * core/apps/positioning/PRSolve.cpp


**Test Changes**

*New/Added Tests*
 * ext/tests/GNSSEph/CNavPackets_T.cpp
 * core/tests/GNSSEph/OrbElemLNav_valid_T.cpp


**Truth Data Changes**

*Updated Truth Data*
 * data/outputs/RinSum_obspath_v211.exp
 * data/outputs/RinSum_v210_osn3.exp
 * data/outputs/RinSum_v211_cags.exp
 * data/outputs/RinSum_v211_kerg.exp
 * data/outputs/RinSum_v211_nklg.exp
 * data/outputs/RinSum_v302_FAA1.exp

Fixes since v2.11
----------------
* RinSum now prints the Marker name as part of the output.  PRSolve also no longer prints an extra line in the output on Windows. It was a matter of using LOGstrm instead of LOG(INFO) in the timing output. REASON: See issues #355 & #356
