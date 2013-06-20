%rename(__str__) *::asString() const;

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


%pythoncode %{
TimeSystems = {
	'Unknown' : 0,
	'Any'     : 1,
	'GPS'     : 2,
	'GLO'     : 3,
	'GAL'     : 4,
	'COM'     : 5,
	'UTC'     : 6,
	'UT1'     : 7,
	'TAI' 	  : 8,
	'TT'      : 9
}
def makeTimeSystem(system='Unknown'):
	return TimeSystem(TimeSystems[system])
%}
	