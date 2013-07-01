%module(docstring="The GPS Toolkit - an open source library to the satellite navigation community.") gpstk
%{
    // time:
    #include "../../../src/TimeSystem.hpp"
    #include "../../../src/TimeTag.hpp"
    #include "../../../src/UnixTime.hpp"
    #include "../../../src/ANSITime.hpp"
    #include "../../../src/CivilTime.hpp"
    #include "../../../src/MathBase.hpp"
    #include "../../../src/CommonTime.hpp"
    #include "../../../src/Exception.hpp"
    #include "../../../src/GPSZcount.hpp"
    #include "../../../src/GPSWeek.hpp"
    #include "../../../src/GPSWeekSecond.hpp"
    #include "../../../src/GPSWeekZcount.hpp"
    #include "../../../src/JulianDate.hpp"
    #include "../../../src/MJD.hpp"
    #include "../../../src/SystemTime.hpp"
    #include "../../../src/TimeConstants.hpp"
    #include "../../../src/TimeConverters.hpp"
    #include "../../../src/TimeString.hpp"
    #include "../../../src/YDSTime.hpp"
    #include "../../../src/Exception.hpp"

    // util:
    #include "../../../src/geometry.hpp"
    #include "../../../src/gps_constants.hpp"
    #include "../../../src/SatID.hpp"
    #include "../../../src/ObsIDInitializer.hpp"
    #include "../../../src/ObsID.hpp"
    #include "../../../src/GNSSconstants.hpp"
    #include "../../../src/Triple.hpp"
    #include "../../../src/ReferenceFrame.hpp"
    #include "../../../src/EllipsoidModel.hpp"
    #include "../../../src/Xvt.hpp"
    #include "../../../src/StringUtils.hpp"
    #include "../../../src/Position.hpp"
    #include "../../../src/Xv.hpp"
    #include "../../../src/convhelp.hpp"

    // math:
    #include "../../../src/SpecialFunctions.hpp"
    #include "../../../src/VectorBase.hpp"
    #include "../../../src/Vector.hpp"
    #include "../../../src/BaseDistribution.hpp"
    #include "../../../src/GaussianDistribution.hpp"
    #include "../../../src/Chi2Distribution.hpp"
    #include "../../../src/StudentDistribution.hpp"
    #include "../../../src/Stats.hpp"


    // ?
    #include "../../../src/XvtStore.hpp"
    #include "../../../src/PZ90Ellipsoid.hpp"
    #include "../../../src/WGS84Ellipsoid.hpp"
    #include "../../../src/gpstkplatform.h"
    #include "../../../src/FFStreamError.hpp"
    #include "../../../src/FileStore.hpp"
    #include "../../../src/BinUtils.hpp"
    #include "../../../src/FFData.hpp"
    #include "../../../src/EngNav.hpp"
    #include "../../../src/YumaBase.hpp"
    #include "../../../src/FFStream.hpp"
    #include "../../../src/FFTextStream.hpp"
    #include "../../../src/AlmOrbit.hpp"
    #include "../../../src/YumaHeader.hpp"
    #include "../../../src/EngAlmanac.hpp"
    #include "../../../src/OrbElemStore.hpp"
    #include "../../../src/YumaStream.hpp"
    #include "../../../src/YumaData.hpp"
    #include "../../../src/GPSAlmanacStore.hpp"
    #include "../../../src/YumaAlmanacStore.hpp"
    #include "../../../src/OrbElemStore.hpp"
    #include "../../../src/SVNumXRef.hpp"
    #include "../../../src/GPSEphemerisStore.hpp"
    #include "../../../src/RinexSatID.hpp"
    #include "../../../src/GPS_URA.hpp"
    #include "../../../src/BrcClockCorrection.hpp"
    #include "../../../src/BrcKeplerOrbit.hpp"
    #include "../../../src/EngEphemeris.hpp"
    #include "../../../src/GalEphemeris.hpp"
    #include "../../../src/GalEphemerisStore.hpp"
    #include "../../../src/RinexClockBase.hpp"
    #include "../../../src/RinexObsBase.hpp"
    #include "../../../src/RinexObsHeader.hpp"
    #include "../../../src/RinexObsID.hpp"
    #include "../../../src/RinexClockHeader.hpp"
    #include "../../../src/RinexClockData.hpp"
    #include "../../../src/RinexObsStream.hpp"


    #include "../../../src/Rinex3ClockBase.hpp"
    #include "../../../src/Rinex3ObsBase.hpp"
    #include "../../../src/Rinex3ObsHeader.hpp"
    #include "../../../src/Rinex3ObsStream.hpp"
    #include "../../../src/Rinex3ClockHeader.hpp"
    #include "../../../src/Rinex3ClockData.hpp"

    #include "../../../src/TabularSatStore.hpp"
    #include "../../../src/ClockSatStore.hpp"
    #include "../../../src/SP3Base.hpp"
    #include "../../../src/SP3SatID.hpp"
    #include "../../../src/SP3Header.hpp"
    #include "../../../src/SP3Data.hpp"
    #include "../../../src/PositionSatStore.hpp"
    #include "../../../src/SP3EphemerisStore.hpp"

    typedef std::map<char, std::string> IdToValue; // defined in TimeTag.hpp
    using namespace gpstk;
%}


// =============================================================
//  Section 1: C++ template containers & typedefs
// =============================================================
%feature("autodoc","1");
%include "doc/doc.i"
%include "std_string.i"
%include "std_map.i"
%rename(streamInput) operator>>;
%rename(__str__) *::asString() const;

// %rename(streamOutput) operator<<;
%include "src/std_vector_extra.i" // renamed since std_vector.i part of core swig
typedef std::map< char, std::string> IdToValue;
%template(map_int_char) std::map<int, char>;
  %template() std::pair<int, char>;
%template(map_int_string) std::map<int, std::string>;
  %template() std::pair<int, std::string>;
%template(map_char_int) std::map<char, int>;
  %template() std::pair<char, int>;
%template(map_string_int) std::map<std::string, int>;
  %template() std::pair<std::string, int>;
%include "src/Exception.i"


// =============================================================
//  Section 2: Time classes
// =============================================================
%include "src/TimeSystem.i"
%include "../../../src/TimeTag.hpp"
%include "../../../src/TimeConstants.hpp"
%include "../../../src/CommonTime.hpp"
%feature("notabstract") UnixTime;
%include "../../../src/UnixTime.hpp"
%feature("notabstract") SystemTime;
%include "../../../src/SystemTime.hpp"
%feature("notabstract") ANSITime;
%include "../../../src/ANSITime.hpp"
%feature("notabstract") CivilTime;
%include "src/CivilTime.i"
%include "../../../src/GPSZcount.hpp"
%include "../../../src/GPSWeek.hpp"
%feature("notabstract") GPSWeekSecond;
%include "../../../src/GPSWeekSecond.hpp"
%feature("notabstract") GPSWeekZcount;
%include "../../../src/GPSWeekZcount.hpp"
%feature("notabstract") JulianDate;
%include "../../../src/JulianDate.hpp"
%feature("notabstract") MJD;
%include "../../../src/MJD.hpp"
%feature("notabstract") YDSTime;
%include "../../../src/YDSTime.hpp"
%include "../../../src/TimeConverters.hpp"
%include "src/TimeString.i"


// =============================================================
//  Section 3: General/Utils classes
// =============================================================
// Utils stuff
%include "../../../src/geometry.hpp"
%include "../../../src/gps_constants.hpp"
%include "src/SatID.i"
%include "../../../src/ObsIDInitializer.hpp"
%include "../../../src/ObsID.hpp"
%include "../../../src/GNSSconstants.hpp"
%include "src/Triple.i"
%include "src/ReferenceFrame.i"
%include "../../../src/EllipsoidModel.hpp"
%include "../../../src/Xvt.hpp"
%include "src/Position.i"
%include "../../../src/convhelp.hpp"
%include "../../../src/Xv.hpp"

%pythoncode %{
ObsID.__str__ = lambda self: asString(self)
Xv.__str__ = lambda self: 'x:'+ self.x.__str__() + ', v:' + self.v.__str__()
def xvt_str(self):
    output = 'x:' + str(self.x)
    output += ', v:' + str(self.v)
    output += ', clk bias:' + str(self.clkbias)
    output += ', clk drift:' + str(self.clkdrift)
    output += ', relcorr:' + str(self.relcorr)
    return output
Xvt.__str__ = xvt_str
%}


// Math stuff
%include "../../../src/SpecialFunctions.hpp"
%include "src/VectorBase.i"
%include "src/Vector.i"
%include "../../../src/BaseDistribution.hpp"
%include "../../../src/GaussianDistribution.hpp"
%include "../../../src/Chi2Distribution.hpp"
%include "../../../src/StudentDistribution.hpp"
%include "src/Stats.i"



// =============================================================
//  Section 4: "XvtStore and friends" (The wild, wild west)
// =============================================================
%include "../../../src/PZ90Ellipsoid.hpp"
%include "../../../src/WGS84Ellipsoid.hpp"

%include "../../../src/XvtStore.hpp"
%template(XvtStore_SatID)  gpstk::XvtStore<gpstk::SatID>;
%template(XvtStore_string) gpstk::XvtStore<std::string>;

%include "../../../src/gpstkplatform.h"
%include "../../../src/FFStreamError.hpp"
%include "../../../src/FileStore.hpp"
%include "../../../src/BinUtils.hpp"
%include "../../../src/FFData.hpp"
%include "../../../src/EngNav.hpp"
%include "../../../src/YumaBase.hpp"
%include "../../../src/FFStream.hpp"
%include "../../../src/FFTextStream.hpp"
%include "../../../src/AlmOrbit.hpp"
%include "../../../src/YumaHeader.hpp"
%include "src/EngAlmanac.i"
%include "../../../src/OrbElemStore.hpp"
%include "../../../src/AlmOrbit.hpp"
%include "../../../src/YumaStream.hpp"
%include "../../../src/YumaData.hpp"
%include "../../../src/GPSAlmanacStore.hpp"
%template(FileStore_YumaHeader) gpstk::FileStore<gpstk::YumaHeader>;
%include "../../../src/YumaAlmanacStore.hpp"

%include "../../../src/OrbElemStore.hpp"
%include "../../../src/SVNumXRef.hpp"
%include "../../../src/GPSEphemerisStore.hpp"
%include "../../../src/RinexSatID.hpp"
%include "../../../src/GPS_URA.hpp"
%include "../../../src/BrcClockCorrection.hpp"
%include "../../../src/BrcKeplerOrbit.hpp"
%include "../../../src/EngEphemeris.hpp"
%include "../../../src/GalEphemeris.hpp"
%include "../../../src/GalEphemerisStore.hpp"

// %include "../../../src/RinexClockBase.hpp"
// %include "../../../src/RinexObsBase.hpp"
// %include "../../../src/RinexObsHeader.hpp"
// %include "../../../src/RinexObsID.hpp"
// %include "../../../src/RinexObsStream.hpp"
// %include "../../../src/RinexClockHeader.hpp"
// %include "../../../src/RinexClockData.hpp"

// %include "../../../src/Rinex3ClockBase.hpp"
// %include "../../../src/Rinex3ObsBase.hpp"
// %include "../../../src/Rinex3ObsHeader.hpp"
// %include "../../../src/Rinex3ObsStream.hpp"

// %include "../../../src/Rinex3ClockHeader.hpp"
// %include "../../../src/Rinex3ClockData.hpp"

// %include "../../../src/TabularSatStore.hpp"

// %include "src/ClockSatStore.i"
// %include "../../../src/SP3Base.hpp"
// %include "../../../src/SP3SatID.hpp"
// %include "../../../src/SP3Header.hpp"
// %include "../../../src/SP3Data.hpp"
// %include "src/PositionSatStore.i"
// %include "../../../src/SP3EphemerisStore.hpp"

// %inline %{
//  gpstk::Rinex3ObsStream makeRinex3ObsStream(std::string file, std::string mode) {
//      Rinex3ObsStream r(); (//"bahr1620.04o.new", std::ios::out|std::ios::trunc);
//      return r;
//  }
// %}