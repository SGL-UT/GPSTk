# $Id$
## Process this file with automake to produce Makefile.in
lib_LTLIBRARIES = libgpstk.la
libgpstk_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libgpstk_la_SOURCES = ANSITime.cpp AlmOrbit.cpp \
Bancroft.cpp BasicFramework.cpp BinUtils.cpp \
BinexData.cpp CivilTime.cpp CodeBuffer.cpp CommandOption.cpp \
CommandOptionParser.cpp CommandOptionWithCommonTimeArg.cpp \
CommandOptionWithPositionArg.cpp CommandOptionWithTimeArg.cpp CommonTime.cpp \
DOP.cpp DayTime.cpp ECEF.cpp EngAlmanac.cpp EngEphemeris.cpp EngNav.cpp \
EphemerisRange.cpp Exception.cpp Expression.cpp FFData.cpp FFStream.cpp \
FICData.cpp FICData109.cpp FICData162.cpp FICData62.cpp FICData9.cpp \
FICHeader.cpp FileHunter.cpp FileSpec.cpp \
GPSAlmanacStore.cpp GPSEphemerisStore.cpp GPSEpochWeekSecond.cpp \
GPSWeekSecond.cpp GPSWeekZcount.cpp GPSZcount.cpp GPSZcount29.cpp \
GPSZcount32.cpp GenXSequence.cpp Geodetic.cpp IonoModel.cpp \
IonoModelStore.cpp JulianDate.cpp LinearClockModel.cpp LoopedFramework.cpp \
MJD.cpp MOPSWeight.cpp MSCData.cpp ModeledPR.cpp ModeledReferencePR.cpp \
ObsClockModel.cpp ObsEpochMap.cpp ObsID.cpp ObsRngDev.cpp PRSolution.cpp \
Position.cpp RACRotation.cpp RinexEphemerisStore.cpp RinexMetData.cpp \
RinexMetHeader.cpp RinexNavData.cpp RinexNavHeader.cpp RinexObsData.cpp \
RinexObsHeader.cpp RinexObsID.cpp RinexSatID.cpp RinexUtilities.cpp \
RungeKutta4.cpp SEMAlmanacStore.cpp SEMData.cpp SEMHeader.cpp SMODFData.cpp \
SP3Data.cpp SP3EphemerisStore.cpp SP3Header.cpp SP3SatID.cpp \
SVExclusionList.cpp SVPCodeGen.cpp SimpleIURAWeight.cpp SolverLMS.cpp \
SolverWMS.cpp SourceID.cpp SystemTime.cpp TabularEphemerisStore.cpp \
TimeConverters.cpp TimeString.cpp TimeTag.cpp Triple.cpp TropModel.cpp \
TypeID.cpp UnixTime.cpp VectorBase.cpp WxObsMap.cpp X1Sequence.cpp \
X2Sequence.cpp Xvt.cpp YDSTime.cpp YumaAlmanacStore.cpp YumaData.cpp \
YumaHeader.cpp DataHeaders.cpp DataStructures.cpp \
ProcessingList.cpp ProcessingVector.cpp SimpleFilter.cpp \
ComputePC.cpp ComputeLC.cpp ComputePI.cpp ComputeLI.cpp \
ComputePdelta.cpp ComputeLdelta.cpp ComputeMelbourneWubbena.cpp \
OneFreqCSDetector.cpp LICSDetector.cpp MWCSDetector.cpp CodeSmoother.cpp \
PCSmoother.cpp ComputeIURAWeights.cpp ComputeMOPSWeights.cpp \
XYZ2NEU.cpp XYZ2NED.cpp DeltaOp.cpp SimpleKalmanFilter.cpp NablaOp.cpp

incldir = $(includedir)/gpstk
incl_HEADERS = ANSITime.hpp AlmOrbit.hpp \
Bancroft.hpp BasicFramework.hpp BinUtils.hpp \
BinexData.hpp BinexFilterOperators.hpp BinexStream.hpp BivarStats.hpp \
CheckPRData.hpp CivilTime.hpp ClockModel.hpp CodeBuffer.hpp \
CommandOption.hpp CommandOptionParser.hpp CommandOptionWithCommonTimeArg.hpp \
CommandOptionWithPositionArg.hpp CommandOptionWithTimeArg.hpp CommonTime.hpp \
DOP.hpp DayTime.hpp ECEF.hpp EngAlmanac.hpp \
EngEphemeris.hpp EngNav.hpp EphemerisRange.hpp \
EpochClockModel.hpp Exception.hpp Expression.hpp ExtractC1.hpp \
ExtractCombinationData.hpp ExtractD1.hpp ExtractD2.hpp ExtractData.hpp \
ExtractL1.hpp ExtractL2.hpp ExtractLC.hpp ExtractP1.hpp ExtractP2.hpp \
ExtractPC.hpp FFBinaryStream.hpp FFData.hpp FFStream.hpp FFStreamError.hpp \
FFTextStream.hpp FICAStream.hpp FICBase.hpp FICData.hpp FICData109.hpp \
FICData162.hpp FICData62.hpp FICData9.hpp FICFilterOperators.hpp \
FICHeader.hpp FICStream.hpp FICStreamBase.hpp FileFilter.hpp \
FileFilterFrame.hpp FileFilterFrameWithHeader.hpp FileHunter.hpp FileSpec.hpp \
FileStore.hpp FileUtils.hpp GPSEphemerisStore.cpp GPSAlmanacStore.hpp \
GPSEpochWeekSecond.hpp GPSGeoid.hpp \
GPSWeekSecond.hpp GPSWeekZcount.hpp GPSZcount.hpp GPSZcount29.hpp \
GPSZcount32.hpp GenXSequence.hpp Geodetic.hpp GeoidModel.hpp IonoModel.hpp \
IonoModelStore.hpp JulianDate.hpp LinearClockModel.hpp LoopedFramework.hpp \
MJD.hpp MOPSWeight.hpp MSCData.hpp MSCStream.hpp MathBase.hpp Matrix.hpp \
MatrixBase.hpp MatrixBaseOperators.hpp MatrixFunctors.hpp \
MatrixImplementation.hpp MatrixOperators.hpp MiscMath.hpp ModeledPR.hpp \
ModeledPseudorangeBase.hpp ModeledReferencePR.hpp ORDEpoch.hpp \
ObsClockModel.hpp ObsEpochMap.hpp ObsID.hpp ObsRngDev.hpp \
PCodeConst.hpp PRSolution.hpp PolyFit.hpp Position.hpp \
RACRotation.hpp RTFileFrame.hpp RinexEphemerisStore.hpp RinexMetBase.hpp \
RinexMetData.hpp RinexMetFilterOperators.hpp RinexMetHeader.hpp \
RinexMetStream.hpp RinexNavBase.hpp RinexNavData.hpp \
RinexNavFilterOperators.hpp RinexNavHeader.hpp RinexNavStream.hpp \
RinexObsBase.hpp RinexObsData.hpp RinexObsFilterOperators.hpp \
RinexObsHeader.hpp RinexObsID.hpp RinexObsStream.hpp RinexSatID.hpp \
RinexUtilities.hpp RungeKutta4.hpp SEMAlmanacStore.hpp SEMBase.hpp \
SEMData.hpp SEMHeader.hpp SEMStream.hpp SMODFData.hpp SMODFStream.hpp \
SP3Base.hpp SP3Data.hpp SP3EphemerisStore.hpp SP3Header.hpp SP3SatID.hpp \
SP3Stream.hpp SVExclusionList.hpp SVPCodeGen.hpp SatID.hpp \
SimpleIURAWeight.hpp SolverBase.hpp SolverLMS.hpp SolverWMS.hpp \
SourceID.hpp Stats.hpp StringUtils.hpp SystemTime.hpp \
TabularEphemerisStore.hpp TimeConstants.hpp TimeConverters.hpp \
TimeNamedFileStream.hpp TimeString.hpp TimeTag.hpp Triple.hpp TropModel.hpp \
TypeID.hpp UnixTime.hpp ValidType.hpp Vector.hpp VectorBase.hpp \
VectorBaseOperators.hpp VectorOperators.hpp WGS84Geoid.hpp WeightBase.hpp \
WxObsMap.hpp X1Sequence.hpp X2Sequence.hpp Xvt.hpp YDSTime.hpp \
YumaAlmanacStore.hpp YumaBase.hpp YumaData.hpp YumaHeader.hpp YumaStream.hpp \
convhelp.hpp geometry.hpp gps_constants.hpp icd_200_constants.hpp \
stl_helpers.hpp DataHeaders.hpp DataStructures.hpp \
ProcessingClass.hpp ProcessingList.hpp ProcessingVector.hpp SimpleFilter.hpp \
ComputeCombination.hpp ComputePC.hpp ComputeLC.hpp ComputePI.hpp \
ComputeLI.hpp ComputePdelta.hpp ComputeLdelta.hpp ComputeIURAWeights.hpp \
ComputeMOPSWeights.hpp ComputeMelbourneWubbena.hpp OneFreqCSDetector.hpp \
CodeSmoother.hpp PCSmoother.hpp LICSDetector.hpp MWCSDetector.hpp \
XYZ2NEU.hpp XYZ2NED.hpp DeltaOp.hpp SimpleKalmanFilter.hpp NablaOp.hpp \
EphemerisStore.hpp AlmanacStore.hpp BCEphemerisStore.hpp