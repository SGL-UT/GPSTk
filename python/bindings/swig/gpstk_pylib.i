%module gpstk_pylib
%{
   // time:
   #include "../../../dev/ext/lib/RefTime/TimeSystem.hpp"
   #include "../../../dev/ext/lib/TimeHandling/TimeTag.hpp"
   #include "../../../dev/ext/lib/TimeHandling/TimeConstants.hpp"
   #include "../../../dev/ext/lib/TimeHandling/TimeConverters.hpp"
   #include "../../../dev/ext/lib/TimeHandling/Week.hpp"
   #include "../../../dev/ext/lib/TimeHandling/WeekSecond.hpp"
   #include "../../../dev/ext/lib/TimeHandling/UnixTime.hpp"
   #include "../../../dev/ext/lib/TimeHandling/ANSITime.hpp"
   #include "../../../dev/ext/lib/TimeHandling/CivilTime.hpp"
   #include "../../../dev/ext/lib/Math/MathBase.hpp"
   #include "../../../dev/ext/lib/TimeHandling/CommonTime.hpp"
   #include "../../../dev/ext/lib/Utilities/Exception.hpp"
   #include "../../../dev/ext/lib/TimeHandling/GPSZcount.hpp"
   #include "../../../dev/ext/lib/TimeHandling/GPSWeek.hpp"
   #include "../../../dev/ext/lib/TimeHandling/GPSWeekSecond.hpp"
   #include "../../../dev/ext/lib/TimeHandling/GPSWeekZcount.hpp"
   #include "../../../dev/ext/lib/TimeHandling/JulianDate.hpp"
   #include "../../../dev/ext/lib/TimeHandling/BDSWeekSecond.hpp"
   #include "../../../dev/ext/lib/TimeHandling/GALWeekSecond.hpp"
   #include "../../../dev/ext/lib/TimeHandling/QZSWeekSecond.hpp"
   #include "../../../dev/ext/lib/TimeHandling/MJD.hpp"
   #include "../../../dev/ext/lib/TimeHandling/SystemTime.hpp"
   #include "../../../dev/ext/lib/TimeHandling/TimeString.hpp"
   #include "../../../dev/ext/lib/TimeHandling/YDSTime.hpp"
   #include "../../../dev/ext/lib/RefTime/TimeSystemCorr.hpp"

   // general files:
   #include "../../../dev/ext/lib/Utilities/StringUtils.hpp"
   #include "../../../dev/ext/lib/GNSSCore/geometry.hpp"
   #include "../../../dev/ext/lib/deprecate/gps_constants.hpp"
   #include "../../../dev/ext/lib/GNSSEph/SatID.hpp"
   #include "../../../dev/ext/lib/GNSSCore/ObsIDInitializer.hpp"
   #include "../../../dev/ext/lib/GNSSCore/ObsID.hpp"
   #include "../../../dev/ext/lib/GNSSCore/GNSSconstants.hpp"
   #include "../../../dev/ext/lib/Math/Triple.hpp"
   #include "../../../dev/ext/lib/RefTime/ReferenceFrame.hpp"
   #include "../../../dev/ext/lib/GNSSCore/EllipsoidModel.hpp"
   #include "../../../dev/ext/lib/GNSSCore/Xvt.hpp"
   #include "../../../dev/ext/lib/Utilities/StringUtils.hpp"
   #include "../../../dev/ext/lib/GNSSCore/Position.hpp"
   #include "../../../dev/ext/lib/GNSSCore/Xv.hpp"
   #include "../../../dev/ext/lib/GNSSCore/convhelp.hpp"
   #include "../../../dev/ext/lib/Math/Vector/VectorBase.hpp"
   #include "../../../dev/ext/lib/Math/Vector/Vector.hpp"
   #include "../../../dev/ext/lib/Procframe/Antenna.hpp"

   // more specific almanac/ephemeris files:
   #include "../../../dev/ext/lib/AstroEph/AstronomicalFunctions.hpp"
   #include "../../../dev/ext/lib/GNSSEph/XvtStore.hpp"
   #include "../../../dev/ext/lib/GNSSCore/PZ90Ellipsoid.hpp"
   #include "../../../dev/ext/lib/GNSSCore/WGS84Ellipsoid.hpp"
   #include "../../../dev/ext/lib/Utilities/gpstkplatform.h"
   #include "../../../dev/ext/lib/FileHandling/FFStreamError.hpp"
   #include "../../../dev/ext/lib/FileDirProc/FileStore.hpp"
   #include "../../../dev/ext/lib/Utilities/BinUtils.hpp"
   #include "../../../dev/ext/lib/FileHandling/FFData.hpp"
   #include "../../../dev/ext/lib/GNSSEph/EngNav.hpp"
   #include "../../../dev/ext/lib/FileHandling/Yuma/YumaBase.hpp"
   #include "../../../dev/ext/lib/FileHandling/FFStream.hpp"
   #include "../../../dev/ext/lib/FileHandling/FFTextStream.hpp"
   #include "../../../dev/ext/lib/GNSSEph/AlmOrbit.hpp"
   #include "../../../dev/ext/lib/FileHandling/Yuma/YumaHeader.hpp"
   #include "../../../dev/ext/lib/GNSSEph/EngAlmanac.hpp"
   #include "../../../dev/ext/lib/GNSSEph/OrbElem.hpp"
   #include "../../../dev/ext/lib/GNSSEph/OrbElemStore.hpp"
   #include "../../../dev/ext/lib/FileHandling/Yuma/YumaStream.hpp"
   #include "../../../dev/ext/lib/FileHandling/Yuma/YumaData.hpp"
   #include "../../../dev/ext/lib/GNSSEph/GPSAlmanacStore.hpp"
   #include "../../../dev/ext/lib/GNSSEph/YumaAlmanacStore.hpp"
   #include "../../../dev/ext/lib/Misc/SVNumXRef.hpp"
   #include "../../../dev/ext/lib/GNSSEph/RinexSatID.hpp"
   #include "../../../dev/ext/lib/GNSSEph/GPS_URA.hpp"
   #include "../../../dev/ext/lib/GNSSEph/BrcClockCorrection.hpp"
   #include "../../../dev/ext/lib/GNSSEph/BrcKeplerOrbit.hpp"
   #include "../../../dev/ext/lib/GNSSEph/EphemerisRange.hpp"
  // #include "../../../dev/ext/lib/Rxio/EphReader.hpp"

   // Ephemeris:
   #include "../../../dev/ext/lib/GNSSEph/BDSEphemeris.hpp"
   #include "../../../dev/ext/lib/GNSSEph/EngEphemeris.hpp"
   #include "../../../dev/ext/lib/GNSSEph/GalEphemeris.hpp"
   #include "../../../dev/ext/lib/GNSSEph/GloEphemeris.hpp"
   #include "../../../dev/ext/lib/GNSSEph/GPSEphemeris.hpp"
   #include "../../../dev/ext/lib/GNSSEph/OrbitEph.hpp"
   #include "../../../dev/ext/lib/GNSSEph/QZSEphemeris.hpp"

   // RINEX format:
   #include "../../../dev/ext/lib/GNSSEph/RinexSatID.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexClockBase.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexObsBase.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexObsHeader.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexObsData.hpp"
   #include "../../../dev/ext/lib/GNSSEph/RinexObsID.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexClockHeader.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexClockData.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexClockStream.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexObsStream.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexNavBase.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexNavHeader.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexNavStream.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexNavData.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexMetBase.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexMetHeader.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexMetStream.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexMetData.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3NavBase.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3NavHeader.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3NavStream.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3NavData.hpp"
   #include "../../../dev/ext/lib/GNSSEph/OrbElemRinex.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ClockBase.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ObsBase.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ObsHeader.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ObsData.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ObsStream.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ClockHeader.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ClockData.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ClockStream.hpp"
   #include "../../../dev/ext/lib/GNSSEph/Rinex3EphemerisStore.hpp"
   #include "../../../dev/ext/lib/GNSSEph/GloEphemerisStore.hpp"

   // Ephemeris stores:
   #include "../../../dev/ext/lib/GNSSEph/OrbitEphStore.hpp"
   #include "../../../dev/ext/lib/GNSSEph/BDSEphemerisStore.hpp"
   #include "../../../dev/ext/lib/GNSSEph/GalEphemerisStore.hpp"
   #include "../../../dev/ext/lib/GNSSEph/GPSEphemerisStore.hpp"
   #include "../../../dev/ext/lib/GNSSEph/QZSEphemerisStore.hpp"
   #include "../../../dev/ext/lib/GNSSEph/RinexEphemerisStore.hpp"

    // SP3 format:
   #include "../../../dev/ext/lib/GNSSEph/TabularSatStore.hpp"
   #include "../../../dev/ext/lib/GNSSEph/ClockSatStore.hpp"
   #include "../../../dev/ext/lib/FileHandling/SP3/SP3Base.hpp"
   #include "../../../dev/ext/lib/GNSSEph/SP3SatID.hpp"
   #include "../../../dev/ext/lib/FileHandling/SP3/SP3Header.hpp"
   #include "../../../dev/ext/lib/FileHandling/SP3/SP3Data.hpp"
   #include "../../../dev/ext/lib/FileHandling/SP3/SP3Stream.hpp"
   #include "../../../dev/ext/lib/GNSSEph/PositionSatStore.hpp"
   #include "../../../dev/ext/lib/GNSSEph/SP3EphemerisStore.hpp"
   #include "../../../dev/ext/lib/FileHandling/RINEX/RinexUtilities.hpp"

   // SEM format:
   #include "../../../dev/ext/lib/FileHandling/SEM/SEMBase.hpp"
   #include "../../../dev/ext/lib/FileHandling/SEM/SEMHeader.hpp"
   #include "../../../dev/ext/lib/FileHandling/SEM/SEMStream.hpp"
   #include "../../../dev/ext/lib/FileHandling/SEM/SEMData.hpp"
   #include "../../../dev/ext/lib/GNSSEph/SEMAlmanacStore.hpp"

   #include "../../../dev/ext/lib/Math/Matrix/Matrix.hpp"
   #include "../../../dev/ext/lib/PosSol/Bancroft.hpp"
   #include "../../../dev/ext/lib/Utilities/ValidType.hpp"
   #include "../../../dev/ext/lib/ClockModel/ObsEpochMap.hpp"
   #include "../../../dev/ext/lib/GNSSCore/WxObsMap.hpp"
   #include "../../../dev/ext/lib/GNSSCore/TropModel.hpp"
   #include "../../../dev/ext/lib/Procframe/CheckPRData.hpp"
   #include "../../../dev/ext/lib/PosSol/PRSolution2.hpp"
   #include "../../../dev/ext/lib/Procframe/ExtractData.hpp"
   #include "../../../dev/ext/lib/Math/Expression.hpp"

   #include "../../../dev/ext/lib/FileHandling/FFBinaryStream.hpp"

   // Positioning/Tides
   #include "../../../dev/ext/lib/AstroEph/MoonPosition.hpp"
   #include "../../../dev/ext/lib/AstroEph/SunPosition.hpp"
   #include "../../../dev/ext/lib/GNSSCore/PoleTides.hpp"
   #include "../../../dev/ext/lib/GNSSCore/SolidTides.hpp"

   using namespace gpstk;
%}


// =============================================================
//  Section 1: C++ template containers & typedefs
// =============================================================
%feature("autodoc", "1");
%include "doc/doc.i"
%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_map.i"
%include "std_list.i"
%include "std_set.i"
%include "std_multimap.i"

%rename(__str__) *::asString() const;

// Ignores on things we can't wrap
%ignore *::dump;  // takes a stream as a parameter
%ignore operator<<;
%ignore operator>>;
%ignore *::operator<<;
%ignore *::operator>>;
%ignore *::operator=;
%ignore *::operator++;
%ignore *::operator--;

%include "src/typemaps.i"
%include "src/STLTemplates.i"
%include "src/STLHelpers.i"


%include "exception.i"
%rename(__str__) gpstk::Exception::what() const; //Rename the Exception output to __str__ from const
%include "../../../../dev/ext/lib/Utilities/Exception.hpp" //Include the header file Exception.hpp
%include "../../../../dev/ext/lib/FileHandling/FFStreamError.hpp" //Include the header file FFStreamError.hpp
%include "src/Exception.i"


// =============================================================
//  Section 2: Time classes
// =============================================================
%rename (toString) *::operator std::string() const;
%rename(toCommonTime) *::convertToCommonTime() const;
%ignore *::operator CommonTime() const;

%ignore gpstk::TimeSystem::TimeSystem(int i); //Ignore the declaration i in TimeSystem
%ignore gpstk::TimeSystem::getTimeSystem(); //Ignore the declaration getTimeSystem in TimeSystem
%include "../../../../dev/ext/lib/RefTime/TimeSystem.hpp" //Get the header file TimeSystem
%include "src/TimeSystem.i"

%include "../../../dev/ext/lib/TimeHandling/TimeTag.hpp"
%include "../../../dev/ext/lib/TimeHandling/TimeConstants.hpp"

%ignore gpstk::CommonTime::get;  // takes non-const values as parameters for output
%include "../../../dev/ext/lib/TimeHandling/CommonTime.hpp"
%include "../../../dev/ext/lib/TimeHandling/Week.hpp"
%include "../../../dev/ext/lib/TimeHandling/WeekSecond.hpp"
%feature("notabstract") UnixTime;
%include "../../../dev/ext/lib/TimeHandling/UnixTime.hpp"
%feature("notabstract") SystemTime;
%include "../../../dev/ext/lib/TimeHandling/SystemTime.hpp"
%feature("notabstract") ANSITime;
%include "../../../dev/ext/lib/TimeHandling/ANSITime.hpp"
%feature("notabstract") CivilTime;
%ignore gpstk::CivilTime::MonthNames; //Ignore declarations matching identifier in class
%ignore gpstk::CivilTime::MonthAbbrevNames;
%include "../../../../dev/ext/lib/TimeHandling/CivilTime.hpp" //Import header file CivilTime.hpp
%include "../../../dev/ext/lib/TimeHandling/GPSZcount.hpp"
%include "../../../dev/ext/lib/TimeHandling/GPSWeek.hpp"
%feature("notabstract") GPSWeekSecond;
%include "../../../dev/ext/lib/TimeHandling/GPSWeekSecond.hpp"
%feature("notabstract") GPSWeekZcount;
%include "../../../dev/ext/lib/TimeHandling/GPSWeekZcount.hpp"
%feature("notabstract") JulianDate;
%include "../../../dev/ext/lib/TimeHandling/JulianDate.hpp"
%feature("notabstract") BDSWeekSecond;
%include "../../../dev/ext/lib/TimeHandling/BDSWeekSecond.hpp"
%feature("notabstract") GALWeekSecond;
%include "../../../dev/ext/lib/TimeHandling/GALWeekSecond.hpp"
%feature("notabstract") QZSWeekSecond;
%include "../../../dev/ext/lib/TimeHandling/QZSWeekSecond.hpp"
%feature("notabstract") MJD;
%include "../../../dev/ext/lib/TimeHandling/MJD.hpp"
%feature("notabstract") YDSTime;
%include "../../../dev/ext/lib/TimeHandling/YDSTime.hpp"
%ignore gpstk::scanTime(TimeTag& btime, const std::string& str, const std::string& fmt); //Ignore the declarations in scanTime of TimeTag and CommonTime and in mixedScanTime of CommonTime
%ignore gpstk::scanTime(CommonTime& btime, const std::string& str, const std::string& fmt);
%ignore gpstk::mixedScanTime(CommonTime& btime, const std::string& str, const std::string& fmt);

%include "../../../../dev/ext/lib/TimeHandling/TimeString.hpp" //Grab header file of TimeString
%include "src/TimeString.i"
%include "../../../dev/ext/lib/RefTime/TimeSystemCorr.hpp"


// =============================================================
//  Section 3: General/Utils classes
// =============================================================
// Utils stuff
%include "../../../dev/ext/lib/GNSSCore/geometry.hpp"
%include "../../../dev/ext/lib/deprecate/gps_constants.hpp"
%include "../../../../dev/ext/lib/GNSSEph/SatID.hpp"
%include "src/SatID.i"
%include "../../../dev/ext/lib/GNSSCore/ObsIDInitializer.hpp"
%include "../../../dev/ext/lib/GNSSCore/ObsID.hpp"
%ignore gpstk::SV_ACCURACY_GLO_INDEX;  // wrapper added in GPS_URA.i
%include "../../../dev/ext/lib/GNSSCore/GNSSconstants.hpp"
%ignore gpstk::Triple::operator[](const size_t index);
%ignore gpstk::Triple::operator()(const size_t index);
%ignore gpstk::Triple::operator*(double right, const Triple& rhs);
%ignore gpstk::Triple::theArray;
%include "../../../dev/ext/lib/Math/Triple.hpp"
%ignore gpstk::ReferenceFrame::ReferenceFrame(int i);
%rename(__str__) gpstk::ReferenceFrame::asString() const;
%include "../../../../dev/ext/lib/RefTime/ReferenceFrame.hpp"
%include "../../../dev/ext/lib/GNSSCore/EllipsoidModel.hpp"
%include "../../../dev/ext/lib/GNSSCore/Xvt.hpp"

%ignore gpstk::Position::convertSphericalToCartesian(const Triple& tpr, Triple& xyz) throw();
%ignore gpstk::Position::convertCartesianToSpherical(const Triple& xyz, Triple& tpr) throw();
%ignore gpstk::Position::convertCartesianToGeodetic(const Triple& xyz, Triple& llh, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertGeodeticToCartesian(const Triple&, llh, Triple& xyz, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertCartesianToGeocentric(const Triple& xyz, Triple& llr) throw();
%ignore gpstk::Position::convertGeocentricToCartesian(const Triple& llr, Triple& xyz) throw();
%ignore gpstk::Position::convertGeocentricToGeodetic(const Triple& llr, Triple& geodeticllr, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertGeodeticToGeocentric(const Triple& geodeticllh, Triple& llr, const double A, const double eccSq) throw();
%include "../../../../dev/ext/lib/GNSSCore/Position.hpp"
%include "src/Position.i"

%include "../../../dev/ext/lib/GNSSCore/convhelp.hpp"
%include "../../../dev/ext/lib/GNSSCore/Xv.hpp"
%ignore gpstk::VectorBase::operator[] (size_t i) const;
%ignore gpstk::VectorBase::operator() (size_t i) const;
%ignore gpstk::RefVectorBaseHelper::zeroTolerance;
%ignore gpstk::RefVectorBaseHelper::perator[] (size_t i);
%ignore gpstk::RefVectorBaseHelper::operator() (size_t i);
%ignore gpstk::RefVectorBaseHelper::zeroize();
%include "../../../../dev/ext/lib/Math/Vector/VectorBase.hpp"
%include "src/Vector.i"


// =============================================================
//  Section 4: "XvtStore and friends"
// =============================================================
// Renames on a few commonly used operators
%rename (toEngEphemeris) *::operator EngEphemeris() const;
%rename (toGalEphemeris) *::operator GalEphemeris() const;
%rename (toGloEphemeris) *::operator GloEphemeris() const;
%rename (toAlmOrbit) *::operator AlmOrbit() const;

%include "../../../dev/ext/lib/AstroEph/AstronomicalFunctions.hpp"
%include "../../../dev/ext/lib/GNSSCore/PZ90Ellipsoid.hpp"
%include "../../../dev/ext/lib/GNSSCore/WGS84Ellipsoid.hpp"

%include "../../../dev/ext/lib/GNSSEph/XvtStore.hpp"
%template(XvtStore_SatID)  gpstk::XvtStore<gpstk::SatID>;
%template(XvtStore_string) gpstk::XvtStore<std::string>;

%include "../../../dev/ext/lib/Utilities/gpstkplatform.h"
%include "../../../dev/ext/lib/FileHandling/FFStreamError.hpp"
%include "../../../dev/ext/lib/FileDirProc/FileStore.hpp"
%include "../../../dev/ext/lib/Utilities/BinUtils.hpp"
%include "../../../dev/ext/lib/FileHandling/FFData.hpp"
%include "../../../dev/ext/lib/GNSSEph/EngNav.hpp"
%include "../../../dev/ext/lib/FileHandling/Yuma/YumaBase.hpp"
%include "../../../dev/ext/lib/FileHandling/FFStream.hpp"
%include "../../../dev/ext/lib/FileHandling/FFTextStream.hpp"
%include "../../../dev/ext/lib/GNSSEph/AlmOrbit.hpp"
%include "../../../dev/ext/lib/FileHandling/Yuma/YumaHeader.hpp"
%ignore gpstk::EngAlmanac::getUTC;
%include "../../../dev/ext/lib/GNSSEph/EngAlmanac.hpp"

%include "../../../dev/ext/lib/GNSSEph/OrbElem.hpp"
%include "../../../dev/ext/lib/GNSSEph/OrbElemStore.hpp"
%include "../../../dev/ext/lib/GNSSEph/AlmOrbit.hpp"
%include "../../../dev/ext/lib/FileHandling/Yuma/YumaStream.hpp"
%include "../../../dev/ext/lib/FileHandling/Yuma/YumaData.hpp"
%include "../../../dev/ext/lib/GNSSEph/GPSAlmanacStore.hpp"
%template(FileStore_YumaHeader) gpstk::FileStore<gpstk::YumaHeader>;
%include "../../../dev/ext/lib/GNSSEph/YumaAlmanacStore.hpp"

%include "../../../dev/ext/lib/Misc/SVNumXRef.hpp"
%include "../../../dev/ext/lib/GNSSEph/RinexSatID.hpp"

%ignore gpstk::SV_ACCURACY_GPS_MIN_INDEX;
%ignore gpstk::SV_ACCURACY_GPS_NOMINAL_INDEX;
%ignore gpstk::SV_ACCURACY_GPS_MAX_INDEX;
%ignore gpstk::SV_CNAV_ACCURACY_GPS_MIN_INDEX;
%ignore gpstk::SV_CNAV_ACCURACY_GPS_NOM_INDEX;
%ignore gpstk::SV_CNAV_ACCURACY_GPS_MAX_INDEX;
%include "../../../../dev/ext/lib/GNSSEph/GPS_URA.hpp"
%include "src/GPS_URA.i"

%include "../../../dev/ext/lib/GNSSEph/BrcClockCorrection.hpp"
%include "../../../dev/ext/lib/GNSSEph/BrcKeplerOrbit.hpp"
%include "../../../dev/ext/lib/GNSSEph/EphemerisRange.hpp"
// %include "../../../dev/ext/lib/Rxio/EphReader.hpp"

// Ephemeris:
%include "../../../dev/ext/lib/GNSSEph/OrbitEph.hpp"
%include "../../../dev/ext/lib/GNSSEph/BDSEphemeris.hpp"
%include "../../../dev/ext/lib/GNSSEph/EngEphemeris.hpp"
%include "../../../dev/ext/lib/GNSSEph/GalEphemeris.hpp"
%include "../../../dev/ext/lib/GNSSEph/GloEphemeris.hpp"
%include "../../../dev/ext/lib/GNSSEph/GPSEphemeris.hpp"
%include "../../../dev/ext/lib/GNSSEph/QZSEphemeris.hpp"

// RINEX format:
%include "../../../dev/ext/lib/GNSSEph/RinexSatID.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexClockBase.hpp"
// RINEX obs:
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexObsBase.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexObsHeader.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexObsData.hpp"
%include "../../../dev/ext/lib/GNSSEph/RinexObsID.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexObsStream.hpp"
// RINEX clock:
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexClockHeader.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexClockData.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexClockStream.hpp"
// RINEX nav:
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexNavBase.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexNavHeader.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexNavStream.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexNavData.hpp"
// RINEX meteorological:
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexMetBase.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexMetHeader.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexMetStream.hpp"

%ignore gpstk::RinexMetData::data;
%include "../../../../dev/ext/lib/FileHandling/RINEX/RinexMetData.hpp"
%include "src/RinexMetData.i"

 // RINEX 3 nav:
%include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3NavBase.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3NavHeader.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3NavStream.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3NavData.hpp"
%include "../../../dev/ext/lib/GNSSEph/OrbElemRinex.hpp"
// RINEX 3 clock/obs:
%include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ClockBase.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ObsBase.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ObsHeader.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ObsData.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ObsStream.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ClockHeader.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ClockData.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX3/Rinex3ClockStream.hpp"
%include "../../../dev/ext/lib/GNSSEph/Rinex3EphemerisStore.hpp"

// Ephemeris stores:
%include "../../../dev/ext/lib/GNSSEph/OrbitEphStore.hpp"
%include "../../../dev/ext/lib/GNSSEph/BDSEphemerisStore.hpp"
%include "../../../dev/ext/lib/GNSSEph/GalEphemerisStore.hpp"
%include "../../../dev/ext/lib/GNSSEph/GloEphemerisStore.hpp"
%include "../../../dev/ext/lib/GNSSEph/GPSEphemerisStore.hpp"
%include "../../../dev/ext/lib/GNSSEph/QZSEphemerisStore.hpp"
%template (FileStore_RinexNavHeader) gpstk::FileStore<gpstk::RinexNavHeader>;
%include "../../../dev/ext/lib/GNSSEph/RinexEphemerisStore.hpp"

// SP3 format:
%include "../../../dev/ext/lib/GNSSEph/TabularSatStore.hpp"
%include "../../../dev/ext/lib/GNSSEph/ClockSatStore.hpp"
%include "../../../dev/ext/lib/FileHandling/SP3/SP3Base.hpp"
%include "../../../dev/ext/lib/GNSSEph/SP3SatID.hpp"
%include "../../../dev/ext/lib/FileHandling/SP3/SP3Header.hpp"
%include "../../../dev/ext/lib/FileHandling/SP3/SP3Data.hpp"
%include "../../../dev/ext/lib/FileHandling/SP3/SP3Stream.hpp"
%include "../../../dev/ext/lib/GNSSEph/PositionSatStore.hpp"
%include "../../../dev/ext/lib/GNSSEph/SP3EphemerisStore.hpp"
%include "../../../dev/ext/lib/FileHandling/RINEX/RinexUtilities.hpp"

// SEM format:
%include "../../../dev/ext/lib/FileHandling/SEM/SEMBase.hpp"
%include "../../../dev/ext/lib/FileHandling/SEM/SEMHeader.hpp"
%include "../../../dev/ext/lib/FileHandling/SEM/SEMStream.hpp"
%include "../../../dev/ext/lib/FileHandling/SEM/SEMData.hpp"
%template(FileStore_SEMHeader) gpstk::FileStore<gpstk::SEMHeader>;
%include "../../../dev/ext/lib/GNSSEph/SEMAlmanacStore.hpp"

%include "../../../dev/ext/lib/Utilities/ValidType.hpp"
%include "../../../dev/ext/lib/ClockModel/ObsEpochMap.hpp"
%include "../../../dev/ext/lib/GNSSCore/WxObsMap.hpp"
%include "../../../dev/ext/lib/GNSSCore/TropModel.hpp"
%include "../../../dev/ext/lib/PosSol/PRSolution2.hpp"
%include "../../../dev/ext/lib/Procframe/ExtractData.hpp"
%ignore gpstk::Expression::print(std::ostream& ostr) const;
%include "../../../dev/ext/lib/Math/Expression.hpp"

// FIC format:
%include "../../../dev/ext/lib/FileHandling/FFBinaryStream.hpp"

%include "../../../dev/ext/lib/AstroEph/MoonPosition.hpp"
%include "../../../dev/ext/lib/AstroEph/SunPosition.hpp"
%include "../../../dev/ext/lib/GNSSCore/PoleTides.hpp"
%include "../../../dev/ext/lib/GNSSCore/SolidTides.hpp"

// Encapsulation of many the __str__, __getitem__, etc. functions to avoid clutter.
// When the only change to a class is adding a simple wrapper, add to pythonfunctions
// instead of creating another small file.
%include "src/pythonfunctions.i"
%include "src/FileIO.i"
