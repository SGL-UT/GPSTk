%module gpstk
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
	#include "../../../src/AlmOrbit.hpp"
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
%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%rename(streamInput) operator>>;
// %rename(streamOutput) operator<<;

typedef std::map< char, std::string> IdToValue;
%template(std_vector_double) std::vector<double>;
%template(std_vector_int) std::vector<int>;
%template(map_int_char) std::map<int, char>;
  %template() std::pair<int, char>; 
%template(map_int_string) std::map<int, std::string>;
  %template() std::pair<int, std::string>; 
%template(map_char_int) std::map<char, int>;
  %template() std::pair<char, int>;
%template(map_string_int) std::map<std::string, int>;
  %template() std::pair<std::string, int>; 

%rename(__str__) gpstk::Exception::dump() const;
%include "../../../src/Exception.hpp"


// =============================================================
//  Section 2: Time classes
// =============================================================
%include "gpstk_time.i"


// =============================================================
//  Section 3: General/Utils classes
// =============================================================
%include "gpstk_util.i"
%include "gpstk_math.i"


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
%include "../../../src/EngAlmanac.hpp"
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

%include "../../../src/RinexClockBase.hpp"
%include "../../../src/RinexObsBase.hpp"
%include "../../../src/RinexObsHeader.hpp"
%include "../../../src/RinexObsID.hpp"
%include "../../../src/RinexObsStream.hpp"
%include "../../../src/RinexClockHeader.hpp"
%include "../../../src/RinexClockData.hpp"

%include "../../../src/Rinex3ClockBase.hpp"
%include "../../../src/Rinex3ObsBase.hpp"
%include "../../../src/Rinex3ObsHeader.hpp"
%include "../../../src/Rinex3ObsStream.hpp"

%include "../../../src/Rinex3ClockHeader.hpp"
%include "../../../src/Rinex3ClockData.hpp"

%include "../../../src/TabularSatStore.hpp"

%include "ClockSatStore.i" // exported to own file for template inheiritance issues
%include "../../../src/SP3Base.hpp"
%include "../../../src/SP3SatID.hpp"
%include "../../../src/SP3Header.hpp"
%include "../../../src/SP3Data.hpp"
%include "PositionSatStore.i" // exported to own file for template inheiritance issues
%include "../../../src/SP3EphemerisStore.hpp"

// %inline %{
// 	gpstk::Rinex3ObsStream makeRinex3ObsStream(std::string file, std::string mode) {
// 		Rinex3ObsStream r(); (//"bahr1620.04o.new", std::ios::out|std::ios::trunc);
// 		return r;
// 	}	
// %}
