%module gpstk

%{
// The header includes are kept in a separate file so they can be used
// to build other swig modules
#include "gpstk_swig.hpp"
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

%include "ValidType.hpp"
%include "typemaps.i"
%include "STLTemplates.i"
%include "STLHelpers.i"

%include "exception.i"
%rename(__str__) gpstk::Exception::what() const;
%include "Exception.hpp"
%include "FFStreamError.hpp"
%include "GPSTkException.i"


// =============================================================
//  Section 2: Time classes
// =============================================================
%rename (toString) *::operator std::string() const;
%rename(toCommonTime) *::convertToCommonTime() const;
%ignore *::operator CommonTime() const;

%ignore gpstk::TimeSystem::TimeSystem(int i);
%ignore gpstk::TimeSystem::getTimeSystem();
%include "TimeSystem.hpp"
%include "TimeSystem.i"

%include "TimeTag.hpp"
%include "TimeConstants.hpp"

 // Long doubles are used in several interfaces but
 // swig really doesn't handle them
%apply double { long double };

%ignore gpstk::CommonTime::get;  // takes non-const values as parameters for output
%include "CommonTime.hpp"
%include "Week.hpp"
%include "WeekSecond.hpp"
%feature("notabstract") UnixTime;
%include "UnixTime.hpp"
%feature("notabstract") SystemTime;
%include "SystemTime.hpp"
%feature("notabstract") ANSITime;
%include "ANSITime.hpp"
%feature("notabstract") CivilTime;
%ignore gpstk::CivilTime::MonthNames;
%ignore gpstk::CivilTime::MonthAbbrevNames;
%include "CivilTime.hpp"
%include "GPSZcount.hpp"
%include "GPSWeek.hpp"
%feature("notabstract") GPSWeekSecond;
%include "GPSWeekSecond.hpp"
%feature("notabstract") GPSWeekZcount;
%include "GPSWeekZcount.hpp"
%feature("notabstract") JulianDate;
%include "JulianDate.hpp"
%feature("notabstract") BDSWeekSecond;
%include "BDSWeekSecond.hpp"
%feature("notabstract") GALWeekSecond;
%include "GALWeekSecond.hpp"
%feature("notabstract") QZSWeekSecond;
%include "QZSWeekSecond.hpp"
%feature("notabstract") MJD;
%include "MJD.hpp"
%feature("notabstract") YDSTime;
%include "YDSTime.hpp"
%ignore gpstk::scanTime(TimeTag& btime, const std::string& str, const std::string& fmt);
%ignore gpstk::scanTime(CommonTime& btime, const std::string& str, const std::string& fmt);
%ignore gpstk::mixedScanTime(CommonTime& btime, const std::string& str, const std::string& fmt);

%include "TimeString.hpp"
%include "TimeString.i"
%include "TimeSystemCorr.hpp"


// =============================================================
//  Section 3: General/Utils classes
// =============================================================
// Utils stuff
%include "geometry.hpp"
%include "gps_constants.hpp"
%include "SatID.hpp"
%include "SatID.i"
%include "ObsIDInitializer.hpp"
%include "ObsID.hpp"
%include "ObsID.i"
%ignore gpstk::SV_ACCURACY_GLO_INDEX;  // wrapper added in GPS_URA.i
%include "GNSSconstants.hpp"
%ignore gpstk::Triple::operator[](const size_t index);
%ignore gpstk::Triple::operator()(const size_t index);
%ignore gpstk::Triple::operator*(double right, const Triple& rhs);
%ignore gpstk::Triple::theArray;
%include "Triple.hpp"
%include "Triple.i"

%ignore gpstk::ReferenceFrame::ReferenceFrame(int i);
%rename(__str__) gpstk::ReferenceFrame::asString() const;
%include "ReferenceFrame.hpp"
%include "EllipsoidModel.hpp"
%include "Xvt.hpp"

%ignore gpstk::Position::convertSphericalToCartesian(const Triple& tpr, Triple& xyz) throw();
%ignore gpstk::Position::convertCartesianToSpherical(const Triple& xyz, Triple& tpr) throw();
%ignore gpstk::Position::convertCartesianToGeodetic(const Triple& xyz, Triple& llh, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertGeodeticToCartesian(const Triple&, llh, Triple& xyz, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertCartesianToGeocentric(const Triple& xyz, Triple& llr) throw();
%ignore gpstk::Position::convertGeocentricToCartesian(const Triple& llr, Triple& xyz) throw();
%ignore gpstk::Position::convertGeocentricToGeodetic(const Triple& llr, Triple& geodeticllr, const double A, const double eccSq) throw();
%ignore gpstk::Position::convertGeodeticToGeocentric(const Triple& geodeticllh, Triple& llr, const double A, const double eccSq) throw();
%include "Position.hpp"
%include "Position.i"

%include "convhelp.hpp"
%include "Xv.hpp"
%ignore gpstk::VectorBase::operator[] (size_t i) const;
%ignore gpstk::VectorBase::operator() (size_t i) const;
%ignore gpstk::RefVectorBaseHelper::zeroTolerance;
%ignore gpstk::RefVectorBaseHelper::perator[] (size_t i);
%ignore gpstk::RefVectorBaseHelper::operator() (size_t i);
%ignore gpstk::RefVectorBaseHelper::zeroize();
%include "VectorBase.hpp"
%include "Vector.i"
%include "DataStatus.hpp"


// =============================================================
//  Section 4: "XvtStore and friends"
// =============================================================
// Renames on a few commonly used operators
%rename (toEngEphemeris) *::operator EngEphemeris() const;
%rename (toGalEphemeris) *::operator GalEphemeris() const;
%rename (toGloEphemeris) *::operator GloEphemeris() const;
%rename (toAlmOrbit) *::operator AlmOrbit() const;

%include "AstronomicalFunctions.hpp"
%include "PZ90Ellipsoid.hpp"
%include "WGS84Ellipsoid.hpp"

%include "XvtStore.hpp"
%template(XvtStore_SatID)  gpstk::XvtStore<gpstk::SatID>;
%template(XvtStore_string) gpstk::XvtStore<std::string>;

%include "gpstkplatform.h"
%include "FFStreamError.hpp"
%include "FileStore.hpp"
%include "FFData.hpp"
%include "EngNav.hpp"
%include "YumaBase.hpp"
// This is to silence warning about not knowing about the fstream base class
namespace std { class fstream {}; }
%include "FFStream.hpp"
%include "FFTextStream.hpp"
%include "AlmOrbit.hpp"
%include "YumaHeader.hpp"
%ignore gpstk::EngAlmanac::getUTC;
%include "EngAlmanac.hpp"

%include "OrbElemBase.hpp"
%include "OrbElem.hpp"
%include "OrbElemStore.hpp"
%include "AlmOrbit.hpp"
%include "YumaStream.hpp"
%include "YumaData.hpp"
%include "GPSAlmanacStore.hpp"
%template(FileStore_YumaHeader) gpstk::FileStore<gpstk::YumaHeader>;
%include "YumaAlmanacStore.hpp"

%include "SVNumXRef.hpp"
%include "RinexSatID.hpp"

%ignore gpstk::SV_ACCURACY_GPS_MIN_INDEX;
%ignore gpstk::SV_ACCURACY_GPS_NOMINAL_INDEX;
%ignore gpstk::SV_ACCURACY_GPS_MAX_INDEX;
%ignore gpstk::SV_CNAV_ACCURACY_GPS_MIN_INDEX;
%ignore gpstk::SV_CNAV_ACCURACY_GPS_NOM_INDEX;
%ignore gpstk::SV_CNAV_ACCURACY_GPS_MAX_INDEX;
%include "GPS_URA.hpp"
%include "GPS_URA.i"

%include "BrcClockCorrection.hpp"
%include "BrcKeplerOrbit.hpp"
%include "EphemerisRange.hpp"
// %include "EphReader.hpp"

// Ephemeris:
%include "OrbitEph.hpp"
%include "BDSEphemeris.hpp"
%include "EngEphemeris.hpp"
%include "GalEphemeris.hpp"
%include "GloEphemeris.hpp"
%include "GPSEphemeris.hpp"
%include "QZSEphemeris.hpp"

// RINEX format:
%include "RinexSatID.hpp"
// RINEX obs:
%include "RinexObsBase.hpp"
%include "RinexObsHeader.hpp"
%include "RinexObsData.hpp"
%include "RinexObsID.hpp"
%include "RinexObsStream.hpp"
// RINEX nav:
%include "RinexNavBase.hpp"
%include "RinexNavHeader.hpp"
%include "RinexNavStream.hpp"
%include "RinexNavData.hpp"
// RINEX meteorological:
%include "RinexMetBase.hpp"
%include "RinexMetHeader.hpp"
%include "RinexMetStream.hpp"

%ignore gpstk::RinexMetData::data;
%include "RinexMetData.hpp"
%include "RinexMetData.i"

 // RINEX 3 nav:
%include "Rinex3NavBase.hpp"
%include "Rinex3NavHeader.hpp"
%include "Rinex3NavStream.hpp"
%include "Rinex3NavData.hpp"
%include "OrbElemRinex.hpp"
// RINEX 3 clock/obs:
%include "Rinex3ClockBase.hpp"
%include "Rinex3ObsBase.hpp"
%include "Rinex3ObsHeader.hpp"
%include "Rinex3ObsData.hpp"
%include "RinexDatum.hpp"
%include "Rinex3ObsStream.hpp"
%include "Rinex3ClockHeader.hpp"
%include "Rinex3ClockData.hpp"
%include "Rinex3ClockStream.hpp"
%include "Rinex3EphemerisStore.hpp"

// Ephemeris stores:
%include "OrbitEphStore.hpp"
%include "BDSEphemerisStore.hpp"
%include "GalEphemerisStore.hpp"
%include "GloEphemerisStore.hpp"
%include "GPSEphemerisStore.hpp"
%include "QZSEphemerisStore.hpp"
%template (FileStore_RinexNavHeader) gpstk::FileStore<gpstk::RinexNavHeader>;
%include "RinexEphemerisStore.hpp"

// SP3 format:
%include "TabularSatStore.hpp"
%include "ClockSatStore.hpp"
%include "SP3Base.hpp"
%include "SP3SatID.hpp"
%include "SP3Header.hpp"
%include "SP3Data.hpp"
%include "SP3Stream.hpp"
%include "PositionSatStore.hpp"
%include "SP3EphemerisStore.hpp"
%include "RinexUtilities.hpp"

// SEM format:
%include "SEMBase.hpp"
%include "SEMHeader.hpp"
%include "SEMStream.hpp"
%include "SEMData.hpp"
%template(FileStore_SEMHeader) gpstk::FileStore<gpstk::SEMHeader>;
%include "SEMAlmanacStore.hpp"

%include "ObsEpochMap.hpp"
%include "WxObsMap.hpp"
%include "TropModel.hpp"
%include "PRSolution2.hpp"
%include "ExtractData.hpp"
%ignore gpstk::Expression::print(std::ostream& ostr) const;
%include "Expression.hpp"

// FIC format:
%include "FFBinaryStream.hpp"

%include "MoonPosition.hpp"
%include "SunPosition.hpp"
%include "PoleTides.hpp"
%include "SolidTides.hpp"

// Geomatics:
%include "Geomatics.i"
%include "SunEarthSatGeometry.hpp"

// Encapsulation of many the __str__, __getitem__, etc. functions to avoid clutter.
// When the only change to a class is adding a simple wrapper, add to pythonfunctions
// instead of creating another small file.
%include "pythonfunctions.i"
%include "FileIO.i"

%pythoncode %{
        # clean-up the dir listing by removing *_swigregister.
        import gpstk
        stuff=None
        to_remove=[]
        for stuff in locals():
            if stuff.endswith('_swigregister'):
                to_remove.append(stuff)
        for stuff in to_remove:
            del locals()[stuff]
        del locals()['stuff']
        del locals()['to_remove']
    %}
