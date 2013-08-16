# lib/procframe/CMakeLists.txt
set (procframe_src
BasicModel.cpp
CodeKalmanSolver.cpp
CodeSmoother.cpp
ComputeCombination.cpp
ComputeDOP.cpp
ComputeIonoModel.cpp
ComputeIURAWeights.cpp
ComputeLC.cpp
ComputeLdelta.cpp
ComputeLI.cpp
ComputeLinear.cpp
ComputeMelbourneWubbena.cpp
ComputeMOPSWeights.cpp
ComputePC.cpp
ComputePdelta.cpp
ComputePI.cpp
ComputeSatPCenter.cpp
ComputeSimpleWeights.cpp
ComputeTropModel.cpp
ComputeWindUp.cpp
ConstraintSystem.cpp
ConvertC1ToP1.cpp
CorrectCodeBiases.cpp 
CorrectObservables.cpp
DataHeaders.cpp
DataStructures.cpp
Decimate.cpp
DeltaOp.cpp
Differentiator.cpp
DoubleOp.cpp
Dumper.cpp
EclipsedSatFilter.cpp
Equation.cpp
EquationSystem.cpp
GeneralConstraint.cpp
GeneralEquations.cpp
GravitationalDelay.cpp
IonexModel.cpp
Keeper.cpp
LICSDetector.cpp
LICSDetector2.cpp
LinearCombinations.cpp
ModeledPR.cpp
ModeledReferencePR.cpp
ModelObs.cpp
ModelObsFixedStation.cpp
MWCSDetector.cpp
NablaOp.cpp
NetworkObsStreams.cpp
OneFreqCSDetector.cpp
PCSmoother.cpp
PhaseCodeAlignment.cpp
ProblemSatFilter.cpp
ProcessingList.cpp
ProcessingVector.cpp
Pruner.cpp
RequireObservables.cpp
SatArcMarker.cpp
SimpleFilter.cpp
SolverGeneral.cpp
SolverLMS.cpp
SolverPPP.cpp
SolverPPPFB.cpp
SolverWMS.cpp
StochasticModel.cpp
Synchronize.cpp
TypeID.cpp
Variable.cpp
XYZ2NED.cpp
XYZ2NEU.cpp
)
    
add_library (procframe ${STADYN} ${procframe_src})
include_directories (${CMAKE_SOURCE_DIR}/src)
install (DIRECTORY . DESTINATION include/gpstk FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
target_link_libraries (procframe gpstk)
install (TARGETS procframe DESTINATION lib)
