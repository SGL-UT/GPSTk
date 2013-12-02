%module gpstk_pylib
%{
   // time:
   #include "../../../lib/RefTime/TimeSystem.hpp"
   #include "../../../lib/TimeHandling/TimeTag.hpp"
   #include "../../../lib/TimeHandling/TimeConstants.hpp"
   #include "../../../lib/TimeHandling/TimeConverters.hpp"
   #include "../../../lib/TimeHandling/Week.hpp"
   #include "../../../lib/TimeHandling/WeekSecond.hpp"
   #include "../../../lib/TimeHandling/UnixTime.hpp"
   #include "../../../lib/TimeHandling/ANSITime.hpp"
   #include "../../../lib/TimeHandling/CivilTime.hpp"
   #include "../../../lib/Math/MathBase.hpp"
   #include "../../../lib/TimeHandling/CommonTime.hpp"
   #include "../../../lib/Utilities/Exception.hpp"
   #include "../../../lib/TimeHandling/GPSZcount.hpp"
   #include "../../../lib/TimeHandling/GPSWeek.hpp"
   #include "../../../lib/TimeHandling/GPSWeekSecond.hpp"
   #include "../../../lib/TimeHandling/GPSWeekZcount.hpp"
   #include "../../../lib/TimeHandling/JulianDate.hpp"
   #include "../../../lib/TimeHandling/BDSWeekSecond.hpp"
   #include "../../../lib/TimeHandling/GALWeekSecond.hpp"
   #include "../../../lib/TimeHandling/QZSWeekSecond.hpp"
   #include "../../../lib/TimeHandling/MJD.hpp"
   #include "../../../lib/TimeHandling/SystemTime.hpp"
   #include "../../../lib/TimeHandling/TimeString.hpp"
   #include "../../../lib/TimeHandling/YDSTime.hpp"
   #include "../../../lib/RefTime/TimeSystemCorr.hpp"

   // general files:
   #include "../../../lib/Utilities/StringUtils.hpp"
   #include "../../../lib/GNSSCore/geometry.hpp"
   #include "../../../lib/deprecate/gps_constants.hpp"
   #include "../../../lib/GNSSEph/SatID.hpp"
   #include "../../../lib/GNSSCore/ObsIDInitializer.hpp"
   #include "../../../lib/GNSSCore/ObsID.hpp"
   #include "../../../lib/GNSSCore/GNSSconstants.hpp"
   #include "../../../lib/Math/Triple.hpp"
   #include "../../../lib/RefTime/ReferenceFrame.hpp"
   #include "../../../lib/GNSSCore/EllipsoidModel.hpp"
   #include "../../../lib/GNSSCore/Xvt.hpp"
   #include "../../../lib/Utilities/StringUtils.hpp"
   #include "../../../lib/GNSSCore/Position.hpp"
   #include "../../../lib/GNSSCore/Xv.hpp"
   #include "../../../lib/GNSSCore/convhelp.hpp"
   #include "../../../lib/Math/Vector/VectorBase.hpp"
   #include "../../../lib/Math/Vector/Vector.hpp"
   #include "../../../lib/ProcLib/Antenna.hpp"

   // more specific almanac/epehemeris files:
   #include "../../../lib/AstroEph/AstronomicalFunctions.hpp"
   #include "../../../lib/GNSSEph/XvtStore.hpp"
   #include "../../../lib/GNSSCore/PZ90Ellipsoid.hpp"
   #include "../../../lib/GNSSCore/WGS84Ellipsoid.hpp"
   #include "../../../lib/Utilities/gpstkplatform.h"
   #include "../../../lib/FileHandling/FFStreamError.hpp"
   #include "../../../lib/FileDirProc/FileStore.hpp"
   #include "../../../lib/Utilities/BinUtils.hpp"
   #include "../../../lib/FileHandling/FFData.hpp"
   #include "../../../lib/GNSSEph/EngNav.hpp"
   #include "../../../lib/FileHandling/Yuma/YumaBase.hpp"
   #include "../../../lib/FileHandling/FFStream.hpp"
   #include "../../../lib/FileHandling/FFTextStream.hpp"
   #include "../../../lib/GNSSEph/AlmOrbit.hpp"
   #include "../../../lib/FileHandling/Yuma/YumaHeader.hpp"
   #include "../../../lib/GNSSEph/EngAlmanac.hpp"
   #include "../../../lib/GNSSEph/OrbElem.hpp"
   #include "../../../lib/GNSSEph/OrbElemStore.hpp"
   #include "../../../lib/FileHandling/Yuma/YumaStream.hpp"
   #include "../../../lib/FileHandling/Yuma/YumaData.hpp"
   #include "../../../lib/GNSSEph/GPSAlmanacStore.hpp"
   #include "../../../lib/GNSSEph/YumaAlmanacStore.hpp"
   #include "../../../lib/Misc/SVNumXRef.hpp"
   #include "../../../lib/GNSSEph/RinexSatID.hpp"
   #include "../../../lib/GNSSEph/GPS_URA.hpp"
   #include "../../../lib/GNSSEph/BrcClockCorrection.hpp"
   #include "../../../lib/GNSSEph/BrcKeplerOrbit.hpp"

   // Ephemeris:
   #include "../../../lib/GNSSEph/EngEphemeris.hpp"
   #include "../../../lib/GNSSEph/GloEphemeris.hpp"

   // RINEX format:
   #include "../../../lib/GNSSEph/RinexSatID.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexClockBase.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexObsBase.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexObsHeader.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexObsData.hpp"
   #include "../../../lib/GNSSEph/RinexObsID.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexClockHeader.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexClockData.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexClockStream.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexObsStream.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexNavBase.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexNavHeader.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexNavStream.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexNavData.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexMetBase.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexMetHeader.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexMetStream.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexMetData.hpp"
   #include "../../../lib/FileHandling/RINEX3/Rinex3NavBase.hpp"
   #include "../../../lib/FileHandling/RINEX3/Rinex3NavHeader.hpp"
   #include "../../../lib/FileHandling/RINEX3/Rinex3NavStream.hpp"
   #include "../../../lib/FileHandling/RINEX3/Rinex3NavData.hpp"
   #include "../../../lib/GNSSEph/OrbElemRinex.hpp"
   #include "../../../lib/FileHandling/RINEX3/Rinex3ClockBase.hpp"
   #include "../../../lib/FileHandling/RINEX3/Rinex3ObsBase.hpp"
   #include "../../../lib/FileHandling/RINEX3/Rinex3ObsHeader.hpp"
   #include "../../../lib/FileHandling/RINEX3/Rinex3ObsData.hpp"
   #include "../../../lib/FileHandling/RINEX3/Rinex3ObsStream.hpp"
   #include "../../../lib/FileHandling/RINEX3/Rinex3ClockHeader.hpp"
   #include "../../../lib/FileHandling/RINEX3/Rinex3ClockData.hpp"
   #include "../../../lib/FileHandling/RINEX3/Rinex3ClockStream.hpp"
   #include "../../../lib/GNSSEph/Rinex3EphemerisStore.hpp"
   #include "../../../lib/GNSSEph/GloEphemerisStore.hpp"

   // Ephemeris stores:
   #include "../../../lib/GNSSEph/OrbitEph.hpp"
   #include "../../../lib/GNSSEph/BDSEphemeris.hpp"
   #include "../../../lib/GNSSEph/GalEphemeris.hpp"
   #include "../../../lib/GNSSEph/GPSEphemeris.hpp"
   #include "../../../lib/GNSSEph/QZSEphemeris.hpp"
   #include "../../../lib/GNSSEph/OrbitEphStore.hpp"
   #include "../../../lib/GNSSEph/BDSEphemerisStore.hpp"
   #include "../../../lib/GNSSEph/GalEphemerisStore.hpp"
   #include "../../../lib/GNSSEph/GalEphemerisStore.hpp"
   #include "../../../lib/GNSSEph/GPSEphemerisStore.hpp"
   #include "../../../lib/GNSSEph/GPSEphemerisStore.hpp"
   #include "../../../lib/GNSSEph/QZSEphemerisStore.hpp"
   #include "../../../lib/GNSSEph/RinexEphemerisStore.hpp"

    // SP3 format:
   #include "../../../lib/GNSSEph/TabularSatStore.hpp"
   #include "../../../lib/GNSSEph/ClockSatStore.hpp"
   #include "../../../lib/FileHandling/SP3/SP3Base.hpp"
   #include "../../../lib/GNSSEph/SP3SatID.hpp"
   #include "../../../lib/FileHandling/SP3/SP3Header.hpp"
   #include "../../../lib/FileHandling/SP3/SP3Data.hpp"
   #include "../../../lib/FileHandling/SP3/SP3Stream.hpp"
   #include "../../../lib/GNSSEph/PositionSatStore.hpp"
   #include "../../../lib/GNSSEph/SP3EphemerisStore.hpp"
   #include "../../../lib/FileHandling/RINEX/RinexUtilities.hpp"

   // SEM format:
   #include "../../../lib/FileHandling/SEM/SEMBase.hpp"
   #include "../../../lib/FileHandling/SEM/SEMHeader.hpp"
   #include "../../../lib/FileHandling/SEM/SEMStream.hpp"
   #include "../../../lib/FileHandling/SEM/SEMData.hpp"
   #include "../../../lib/GNSSEph/SEMAlmanacStore.hpp"

   #include "../../../lib/Math/Matrix/Matrix.hpp"
   #include "../../../lib/Misc/Bancroft.hpp"
   #include "../../../lib/Utilities/ValidType.hpp"
   #include "../../../lib/ProcLib/ObsEpochMap.hpp"
   #include "../../../lib/GNSSCore/WxObsMap.hpp"
   #include "../../../lib/GNSSCore/TropModel.hpp"
   #include "../../../lib/ProcLib/CheckPRData.hpp"
   #include "../../../lib/PosSol/PRSolution2.hpp"
   #include "../../../lib/ProcLib/ExtractData.hpp"
   #include "../../../lib/Math/Expression.hpp"

   // FIC format:
   #include "../../../lib/FileHandling/FFBinaryStream.hpp"
   #include "../../../lib/FileHandling/FIC/FICBase.hpp"
   #include "../../../lib/FileHandling/FIC/FICStreamBase.hpp"
   #include "../../../lib/FileHandling/FIC/FICStream.hpp"
   #include "../../../lib/FileHandling/FIC/FICHeader.hpp"
   #include "../../../lib/FileHandling/FIC/FICData.hpp"

   // MSC format:
   #include "../../../lib/FileHandling/MSC/MSCBase.hpp"
   #include "../../../lib/FileHandling/MSC/MSCHeader.hpp"
   #include "../../../lib/FileHandling/MSC/MSCData.hpp"
   #include "../../../lib/FileHandling/MSC/MSCStream.hpp"
   #include "../../../lib/GNSSEph/MSCStore.hpp"

   // Positioning/Tides
   #include "../../../lib/AstroEph/MoonPosition.hpp"
   #include "../../../lib/AstroEph/SunPosition.hpp"
   #include "../../../lib/GNSSCore/PoleTides.hpp"
   #include "../../../lib/GNSSCore/SolidTides.hpp"

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
%include "src/Exception.i"


// =============================================================
//  Section 2: Time classes
// =============================================================
%rename (toString) *::operator std::string() const;
%rename(toCommonTime) *::convertToCommonTime() const;
%ignore *::operator CommonTime() const;

%include "src/TimeSystem.i"
%include "../../../lib/TimeHandling/TimeTag.hpp"
%include "../../../lib/TimeHandling/TimeConstants.hpp"

%ignore gpstk::CommonTime::get;  // takes non-const values as parameters for output
%include "../../../lib/TimeHandling/CommonTime.hpp"
%include "../../../lib/TimeHandling/Week.hpp"
%include "../../../lib/TimeHandling/WeekSecond.hpp"
%feature("notabstract") UnixTime;
%include "../../../lib/TimeHandling/UnixTime.hpp"
%feature("notabstract") SystemTime;
%include "../../../lib/TimeHandling/SystemTime.hpp"
%feature("notabstract") ANSITime;
%include "../../../lib/TimeHandling/ANSITime.hpp"
%feature("notabstract") CivilTime;
%include "src/CivilTime.i"
%include "../../../lib/TimeHandling/GPSZcount.hpp"
%include "../../../lib/TimeHandling/GPSWeek.hpp"
%feature("notabstract") GPSWeekSecond;
%include "../../../lib/TimeHandling/GPSWeekSecond.hpp"
%feature("notabstract") GPSWeekZcount;
%include "../../../lib/TimeHandling/GPSWeekZcount.hpp"
%feature("notabstract") JulianDate;
%include "../../../lib/TimeHandling/JulianDate.hpp"
%feature("notabstract") BDSWeekSecond;
%include "../../../lib/TimeHandling/BDSWeekSecond.hpp"
%feature("notabstract") GALWeekSecond;
%include "../../../lib/TimeHandling/GALWeekSecond.hpp"
%feature("notabstract") QZSWeekSecond;
%include "../../../lib/TimeHandling/QZSWeekSecond.hpp"
%feature("notabstract") MJD;
%include "../../../lib/TimeHandling/MJD.hpp"
%feature("notabstract") YDSTime;
%include "../../../lib/TimeHandling/YDSTime.hpp"
%include "src/TimeString.i"
%include "../../../lib/RefTime/TimeSystemCorr.hpp"


// =============================================================
//  Section 3: General/Utils classes
// =============================================================
// Utils stuff
%include "../../../lib/GNSSCore/geometry.hpp"
%include "../../../lib/deprecate/gps_constants.hpp"
%include "src/SatID.i"
%include "../../../lib/GNSSCore/ObsIDInitializer.hpp"
%include "../../../lib/GNSSCore/ObsID.hpp"
%ignore gpstk::SV_ACCURACY_GLO_INDEX;  // wrapper added in GPS_URA.i
%include "../../../lib/GNSSCore/GNSSconstants.hpp"
%include "../../../lib/Math/Triple.hpp"
%include "src/ReferenceFrame.i"
%include "../../../lib/GNSSCore/EllipsoidModel.hpp"
%include "../../../lib/GNSSCore/Xvt.hpp"
%include "src/Position.i"
%include "../../../lib/GNSSCore/convhelp.hpp"
%include "../../../lib/GNSSCore/Xv.hpp"
%include "src/VectorBase.i"
%include "src/Vector.i"


// =============================================================
//  Section 4: "XvtStore and friends"
// =============================================================
// Renames on a few commonly used operators
%rename (toEngEphemeris) *::operator EngEphemeris() const;
%rename (toGalEphemeris) *::operator GalEphemeris() const;
%rename (toGloEphemeris) *::operator GloEphemeris() const;
%rename (toAlmOrbit) *::operator AlmOrbit() const;

%include "../../../lib/AstroEph/AstronomicalFunctions.hpp"
%include "../../../lib/GNSSCore/PZ90Ellipsoid.hpp"
%include "../../../lib/GNSSCore/WGS84Ellipsoid.hpp"

%include "../../../lib/GNSSEph/XvtStore.hpp"
%template(XvtStore_SatID)  gpstk::XvtStore<gpstk::SatID>;
%template(XvtStore_string) gpstk::XvtStore<std::string>;

%include "../../../lib/Utilities/gpstkplatform.h"
%include "../../../lib/FileHandling/FFStreamError.hpp"
%include "../../../lib/FileDirProc/FileStore.hpp"
%include "../../../lib/Utilities/BinUtils.hpp"
%include "../../../lib/FileHandling/FFData.hpp"
%include "../../../lib/GNSSEph/EngNav.hpp"
%include "../../../lib/FileHandling/Yuma/YumaBase.hpp"
%include "../../../lib/FileHandling/FFStream.hpp"
%include "../../../lib/FileHandling/FFTextStream.hpp"
%include "../../../lib/GNSSEph/AlmOrbit.hpp"
%include "../../../lib/FileHandling/Yuma/YumaHeader.hpp"
%ignore gpstk::EngAlmanac::getUTC;
%include "../../../lib/GNSSEph/EngAlmanac.hpp"

%include "../../../lib/GNSSEph/OrbElem.hpp"
%include "../../../lib/GNSSEph/OrbElemStore.hpp"
%include "../../../lib/GNSSEph/AlmOrbit.hpp"
%include "../../../lib/FileHandling/Yuma/YumaStream.hpp"
%include "../../../lib/FileHandling/Yuma/YumaData.hpp"
%include "../../../lib/GNSSEph/GPSAlmanacStore.hpp"
%template(FileStore_YumaHeader) gpstk::FileStore<gpstk::YumaHeader>;
%include "../../../lib/GNSSEph/YumaAlmanacStore.hpp"

%include "../../../lib/Misc/SVNumXRef.hpp"
%include "../../../lib/GNSSEph/RinexSatID.hpp"
%include "src/GPS_URA.i"
%include "../../../lib/GNSSEph/BrcClockCorrection.hpp"
%include "../../../lib/GNSSEph/BrcKeplerOrbit.hpp"

// Ephemeris:
%include "../../../lib/GNSSEph/OrbitEph.hpp"
%include "../../../lib/GNSSEph/BDSEphemeris.hpp"
%include "../../../lib/GNSSEph/EngEphemeris.hpp"
%include "../../../lib/GNSSEph/GalEphemeris.hpp"
%include "../../../lib/GNSSEph/GloEphemeris.hpp"
%include "../../../lib/GNSSEph/GPSEphemeris.hpp"
%include "../../../lib/GNSSEph/QZSEphemeris.hpp"

// RINEX format:
%include "../../../lib/GNSSEph/RinexSatID.hpp"
%include "../../../lib/FileHandling/RINEX/RinexClockBase.hpp"
// RINEX obs:
%include "../../../lib/FileHandling/RINEX/RinexObsBase.hpp"
%include "../../../lib/FileHandling/RINEX/RinexObsHeader.hpp"
%include "../../../lib/FileHandling/RINEX/RinexObsData.hpp"
%include "../../../lib/GNSSEph/RinexObsID.hpp"
%include "../../../lib/FileHandling/RINEX/RinexObsStream.hpp"
// RINEX clock:
%include "../../../lib/FileHandling/RINEX/RinexClockHeader.hpp"
%include "../../../lib/FileHandling/RINEX/RinexClockData.hpp"
%include "../../../lib/FileHandling/RINEX/RinexClockStream.hpp"
// RINEX nav:
%include "../../../lib/FileHandling/RINEX/RinexNavBase.hpp"
%include "../../../lib/FileHandling/RINEX/RinexNavHeader.hpp"
%include "../../../lib/FileHandling/RINEX/RinexNavStream.hpp"
%include "../../../lib/FileHandling/RINEX/RinexNavData.hpp"
// RINEX meteorological:
%include "../../../lib/FileHandling/RINEX/RinexMetBase.hpp"
%include "../../../lib/FileHandling/RINEX/RinexMetHeader.hpp"
%include "../../../lib/FileHandling/RINEX/RinexMetStream.hpp"
%include "src/RinexMetData.i"
// RINEX 3 nav:
%include "../../../lib/FileHandling/RINEX3/Rinex3NavBase.hpp"
%include "../../../lib/FileHandling/RINEX3/Rinex3NavHeader.hpp"
%include "../../../lib/FileHandling/RINEX3/Rinex3NavStream.hpp"
%include "../../../lib/FileHandling/RINEX3/Rinex3NavData.hpp"
%include "../../../lib/GNSSEph/OrbElemRinex.hpp"
// RINEX 3 clock/obs:
%include "../../../lib/FileHandling/RINEX3/Rinex3ClockBase.hpp"
%include "../../../lib/FileHandling/RINEX3/Rinex3ObsBase.hpp"
%include "../../../lib/FileHandling/RINEX3/Rinex3ObsHeader.hpp"
%include "../../../lib/FileHandling/RINEX3/Rinex3ObsData.hpp"
%include "../../../lib/FileHandling/RINEX3/Rinex3ObsStream.hpp"
%include "../../../lib/FileHandling/RINEX3/Rinex3ClockHeader.hpp"
%include "../../../lib/FileHandling/RINEX3/Rinex3ClockData.hpp"
%include "../../../lib/FileHandling/RINEX3/Rinex3ClockStream.hpp"
%include "../../../lib/GNSSEph/Rinex3EphemerisStore.hpp"

// Ephemeris stores:
%include "../../../lib/GNSSEph/OrbitEphStore.hpp"
%include "../../../lib/GNSSEph/BDSEphemerisStore.hpp"
%include "../../../lib/GNSSEph/GalEphemerisStore.hpp"
%include "../../../lib/GNSSEph/GloEphemerisStore.hpp"
%include "../../../lib/GNSSEph/GPSEphemerisStore.hpp"
%include "../../../lib/GNSSEph/QZSEphemerisStore.hpp"
%template (FileStore_RinexNavHeader) gpstk::FileStore<gpstk::RinexNavHeader>;
%include "../../../lib/GNSSEph/RinexEphemerisStore.hpp"

// SP3 format:
%include "../../../lib/GNSSEph/TabularSatStore.hpp"
%include "../../../lib/GNSSEph/ClockSatStore.hpp"
%include "../../../lib/FileHandling/SP3/SP3Base.hpp"
%include "../../../lib/GNSSEph/SP3SatID.hpp"
%include "../../../lib/FileHandling/SP3/SP3Header.hpp"
%include "../../../lib/FileHandling/SP3/SP3Data.hpp"
%include "../../../lib/FileHandling/SP3/SP3Stream.hpp"
%include "../../../lib/GNSSEph/PositionSatStore.hpp"
%include "../../../lib/GNSSEph/SP3EphemerisStore.hpp"
%include "../../../lib/FileHandling/RINEX/RinexUtilities.hpp"

// SEM format:
%include "../../../lib/FileHandling/SEM/SEMBase.hpp"
%include "../../../lib/FileHandling/SEM/SEMHeader.hpp"
%include "../../../lib/FileHandling/SEM/SEMStream.hpp"
%include "../../../lib/FileHandling/SEM/SEMData.hpp"
%template(FileStore_SEMHeader) gpstk::FileStore<gpstk::SEMHeader>;
%include "../../../lib/GNSSEph/SEMAlmanacStore.hpp"

%include "../../../lib/Utilities/ValidType.hpp"
%include "../../../lib/ProcLib/ObsEpochMap.hpp"
%include "../../../lib/GNSSCore/WxObsMap.hpp"
%include "../../../lib/GNSSCore/TropModel.hpp"
%include "../../../lib/PosSol/PRSolution2.hpp"
%include "../../../lib/ProcLib/ExtractData.hpp"
%ignore gpstk::Expression::print(std::ostream& ostr) const;
%include "../../../lib/Math/Expression.hpp"

// FIC format:
%include "../../../lib/FileHandling/FFBinaryStream.hpp"
%include "../../../lib/FileHandling/FIC/FICBase.hpp"
%include "../../../lib/FileHandling/FIC/FICStreamBase.hpp"
%include "../../../lib/FileHandling/FIC/FICStream.hpp"
%include "../../../lib/FileHandling/FIC/FICHeader.hpp"
%include "../../../lib/FileHandling/FIC/FICData.hpp"

// MSC format:
%include "../../../lib/FileHandling/MSC/MSCBase.hpp"
%include "../../../lib/FileHandling/MSC/MSCHeader.hpp"
%include "../../../lib/FileHandling/MSC/MSCData.hpp"
%include "../../../lib/FileHandling/MSC/MSCStream.hpp"
%template(FileStore_MSCHeader) gpstk::FileStore<gpstk::MSCHeader>;
%include "../../../lib/GNSSEph/MSCStore.hpp"

%include "../../../lib/AstroEph/MoonPosition.hpp"
%include "../../../lib/AstroEph/SunPosition.hpp"
%include "../../../lib/GNSSCore/PoleTides.hpp"
%include "../../../lib/GNSSCore/SolidTides.hpp"

// Encapsulation of many the __str__, __getitem__, etc. functions to avoid clutter.
// When the only change to a class is adding a simple wrapper, add to pythonfunctions
// instead of creating another small file.
%include "src/pythonfunctions.i"
%include "src/FileIO.i"
