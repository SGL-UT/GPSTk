# $Id$
## Process this file with automake to produce Makefile.in
#
# Please add entries in alphabetical order 
#
lib_LTLIBRARIES = libgpstk.la
libgpstk_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libgpstk_la_SOURCES = ANSITime.cpp AlmOrbit.cpp \
AstronomicalFunctions.cpp Bancroft.cpp BasicFramework.cpp BinUtils.cpp \
BinexData.cpp  BLQDataReader.cpp Chi2Distribution.cpp CivilTime.cpp \
CodeBuffer.cpp CommandOption.cpp CommandOptionParser.cpp \
CommandOptionWithCommonTimeArg.cpp CommandOptionWithPositionArg.cpp \
CommandOptionWithTimeArg.cpp CommonTime.cpp ConfDataReader.cpp DOP.cpp \
DayTime.cpp ECEF.cpp EngAlmanac.cpp EngEphemeris.cpp EngNav.cpp ENUUtil.cpp \
EphemerisRange.cpp Exception.cpp Expression.cpp FFData.cpp FFStream.cpp \
FICData.cpp FICData109.cpp FICData162.cpp FICData62.cpp FICData9.cpp \
FICHeader.cpp FileHunter.cpp FileSpec.cpp \
GPSAlmanacStore.cpp GPSEphemerisStore.cpp GPSWeek.cpp \
GPSWeekSecond.cpp GPSWeekZcount.cpp GPSZcount.cpp GaussianDistribution.cpp \
GenXSequence.cpp Geodetic.cpp \
IonexData.cpp IonexHeader.cpp IonexStore.cpp IonoModel.cpp \
IonoModelStore.cpp JulianDate.cpp LinearClockModel.cpp LoopedFramework.cpp \
MJD.cpp MOPSWeight.cpp MSCData.cpp MSCStore.cpp \
MoonPosition.cpp NEDUtil.cpp ObsClockModel.cpp ObsEpochMap.cpp ObsID.cpp \
ObsRngDev.cpp OceanLoading.cpp \
PRSolution.cpp PoleTides.cpp Position.cpp PowerSum.cpp RACRotation.cpp \
RinexEphemerisStore.cpp RinexMetData.cpp RinexMetHeader.cpp RinexNavData.cpp \
RinexNavHeader.cpp RinexObsData.cpp RinexObsHeader.cpp RinexObsID.cpp \
RinexSatID.cpp RinexUtilities.cpp RungeKutta4.cpp SEMAlmanacStore.cpp \
SEMData.cpp SEMHeader.cpp SMODFData.cpp SP3Data.cpp SP3EphemerisStore.cpp \
SP3Header.cpp SP3SatID.cpp SVExclusionList.cpp SVNumXRef.cpp SVPCodeGen.cpp \
SatDataReader.cpp SimpleIURAWeight.cpp SimpleKalmanFilter.cpp SolidTides.cpp \
SourceID.cpp SpecialFunctions.cpp SunPosition.cpp SystemTime.cpp \
TabularEphemerisStore.cpp TimeConverters.cpp TimeString.cpp TimeTag.cpp \
Triple.cpp TropModel.cpp TypeID.cpp UnixTime.cpp VectorBase.cpp WxObsMap.cpp \
X1Sequence.cpp X2Sequence.cpp Xvt.cpp YDSTime.cpp YumaAlmanacStore.cpp \
YumaData.cpp

incldir = $(includedir)/gpstk
incl_HEADERS = ANSITime.hpp AllanDeviation.hpp AlmOrbit.hpp \
AstronomicalFunctions.hpp BaseDistribution.hpp \
Bancroft.hpp BasicFramework.hpp BinUtils.hpp BinexData.hpp \
BinexFilterOperators.hpp BinexStream.hpp BivarStats.hpp BLQDataReader.hpp \
CheckPRData.hpp Chi2Distribution.hpp CivilTime.hpp ClockModel.hpp \
CodeBuffer.hpp CommandOption.hpp CommandOptionParser.hpp \
CommandOptionWithCommonTimeArg.hpp CommandOptionWithPositionArg.hpp \
CommandOptionWithTimeArg.hpp CommonTime.hpp ConfDataReader.hpp DOP.hpp \
DayTime.hpp ECEF.hpp EngAlmanac.hpp EngEphemeris.hpp EngNav.hpp ENUUtil.hpp \
EphemerisRange.hpp EpochClockModel.hpp Exception.hpp Expression.hpp \
ExtractC1.hpp ExtractCombinationData.hpp ExtractD1.hpp ExtractD2.hpp \
ExtractData.hpp ExtractL1.hpp ExtractL2.hpp ExtractLC.hpp ExtractP1.hpp \
ExtractP2.hpp ExtractPC.hpp FFBinaryStream.hpp FFData.hpp FFStream.hpp \
FFStreamError.hpp FFTextStream.hpp FICAStream.hpp FICBase.hpp FICData.hpp \
FICData109.hpp FICData162.hpp FICData62.hpp FICData9.hpp \
FICFilterOperators.hpp FICHeader.hpp FICStream.hpp FICStreamBase.hpp \
FileFilter.hpp FileFilterFrame.hpp FileFilterFrameWithHeader.hpp \
FileHunter.hpp FileSpec.hpp FileStore.hpp FileUtils.hpp GPSAlmanacStore.hpp \
GPSEphemerisStore.hpp GPSWeek.hpp GPSGeoid.hpp \
GPSWeekSecond.hpp GPSWeekZcount.hpp GPSZcount.hpp GaussianDistribution.hpp\
GenXSequence.hpp Geodetic.hpp GeoidModel.hpp InOutFramework.hpp \
IonexBase.hpp IonexData.hpp IonexHeader.hpp IonexStore.hpp IonexStream.hpp \
IonoModel.hpp IonoModelStore.hpp JulianDate.hpp \
LinearClockModel.hpp LoopedFramework.hpp MJD.hpp MOPSWeight.hpp MSCBase.hpp \
MSCData.hpp MSCHeader.hpp MSCStore.hpp MSCStream.hpp MathBase.hpp Matrix.hpp \
MatrixBase.hpp MatrixBaseOperators.hpp MatrixFunctors.hpp \
MatrixImplementation.hpp MatrixOperators.hpp MiscMath.hpp \
ModeledPseudorangeBase.hpp MoonPosition.hpp NEDUtil.hpp ORDEpoch.hpp \
ObsClockModel.hpp ObsEpochMap.hpp ObsID.hpp ObsRngDev.hpp OceanLoading.hpp \
PCodeConst.hpp PRSolution.hpp PoleTides.hpp PolyFit.hpp Position.hpp \
PowerSum.hpp RACRotation.hpp RTFileFrame.hpp RinexEphemerisStore.hpp \
RinexMetBase.hpp RinexMetData.hpp \
RinexMetFilterOperators.hpp RinexMetHeader.hpp RinexMetStream.hpp \
RinexNavBase.hpp RinexNavData.hpp RinexNavFilterOperators.hpp \
RinexNavHeader.hpp RinexNavStream.hpp RinexObsBase.hpp RinexObsData.hpp \
RinexObsFilterOperators.hpp RinexObsHeader.hpp RinexObsID.hpp \
RinexObsStream.hpp RinexSatID.hpp RinexUtilities.hpp RungeKutta4.hpp \
SatDataReader.hpp SEMAlmanacStore.hpp SEMBase.hpp SEMData.hpp \
SEMHeader.hpp SEMStream.hpp \
SMODFData.hpp SMODFStream.hpp SP3Base.hpp SP3Data.hpp SP3EphemerisStore.hpp \
SP3Header.hpp SP3SatID.hpp SP3Stream.hpp SVExclusionList.hpp SVNumXRef.hpp \
SVPCodeGen.hpp SatID.hpp SimpleIURAWeight.hpp SimpleKalmanFilter.hpp \
SolidTides.hpp SolverBase.hpp SourceID.hpp SpecialFunctions.hpp Stats.hpp \
StringUtils.hpp SunPosition.hpp SystemTime.hpp TabularEphemerisStore.hpp \
TimeConstants.hpp TimeConverters.hpp TimeNamedFileStream.hpp TimeString.hpp \
TimeTag.hpp Triple.hpp TropModel.hpp TypeID.hpp UnixTime.hpp ValidType.hpp \
Vector.hpp VectorBase.hpp VectorBaseOperators.hpp VectorOperators.hpp \
WGS84Geoid.hpp WeightBase.hpp WxObsMap.hpp X1Sequence.hpp X2Sequence.hpp \
Xvt.hpp XvtStore.hpp YDSTime.hpp YumaAlmanacStore.hpp YumaBase.hpp \
YumaData.hpp YumaHeader.hpp YumaStream.hpp \
convhelp.hpp geometry.hpp gps_constants.hpp gpstkplatform.h \
icd_200_constants.hpp mergePCodeWords.h stl_helpers.hpp
