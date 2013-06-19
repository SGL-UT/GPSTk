// SWIG interface for the gpstk_time module

%module gpstk
%{
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
	#include "../../../src/MathBase.hpp"
	#include "../../../src/Exception.hpp"
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
	#include "../../../src/SpecialFunctions.hpp"
	#include "../../../src/Xv.hpp"
	#include "../../../src/convhelp.hpp"	
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
	typedef std::map<char, std::string> IdToValue; // defined in TimeTag.hpp
	using namespace gpstk;
%}


// =============================================================
//
//  Section 1: C++ template containers & typedefs
//
// =============================================================
%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
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




// =============================================================
//
//  Section 2: Time classes
//
// =============================================================
%include "../../../src/TimeSystem.hpp"
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
%include "../../../src/CivilTime.hpp"
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
%include "../../../src/TimeString.hpp"
%include "../../../src/Exception.hpp"

%inline %{
   gpstk::CommonTime getTime( const std::string& str,
                              const std::string& fmt )
      throw( gpstk::InvalidRequest,
             gpstk::StringUtils::StringException ) {
      gpstk::CommonTime m;
      gpstk::scanTime(m, str, fmt);
      return m;
   }
%}




// =============================================================
//
//  Section 3: General/Utils classes
//
// =============================================================
%include "../../../src/geometry.hpp"
%include "../../../src/gps_constants.hpp"
%include "../../../src/SatID.hpp"
%include "../../../src/ObsIDInitializer.hpp"
%include "../../../src/ObsID.hpp"
%include "../../../src/GNSSconstants.hpp"

%include "../../../src/Triple.hpp"
%extend gpstk::Triple {
    double __getitem__(unsigned int i) {
        return $self->theArray[i];
	} 
	gpstk::Triple scale(double scalar) {
		return Triple(scalar * $self->theArray[0], 
				      scalar * $self->theArray[1], 
				      scalar * $self->theArray[2]);
	}
	std::string __str__() {
		return "(" + std::to_string($self->theArray[0]) + ", " 
				   + std::to_string($self->theArray[1]) + ", " 
				   + std::to_string($self->theArray[2]) + ")";
	}		
}

%include "../../../src/ReferenceFrame.hpp"
%include "../../../src/EllipsoidModel.hpp"
%include "../../../src/Xvt.hpp"
%include "../../../src/Position.hpp"
%include "../../../src/convhelp.hpp"
%include "../../../src/SpecialFunctions.hpp"
%include "../../../src/Xv.hpp"




// =============================================================
//
//  Section 4: "XvtStore and friends" (The wild, wild west)
//
// =============================================================

%include "../../../src/PZ90Ellipsoid.hpp"
%include "../../../src/WGS84Ellipsoid.hpp"

%include "../../../src/XvtStore.hpp"
%template(XvtStore_SatID)  gpstk::XvtStore<SatID>;
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
// %template(FileStore_YumaHeader) gpstk::FileStore<YumaHeader>;	
%include "../../../src/YumaAlmanacStore.hpp"

%include "../../../src/OrbElemStore.hpp"
%include "../../../src/SVNumXRef.hpp"
%include "../../../src/GPSEphemerisStore.hpp"
%include "../../../src/RinexSatID.hpp"
%include "../../../src/GPS_URA.hpp"
%include "../../../src/BrcClockCorrection.hpp"
%include "../../../src/BrcKeplerOrbit.hpp"
%include "../../../src/EngEphemeris.hpp"

%include "gpstk_python_extensions.i"