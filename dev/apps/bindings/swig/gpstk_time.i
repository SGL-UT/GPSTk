// SWIG interface for the gpstk_time module

%include "std_string.i"

%module gpstk_time
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
	typedef std::map<char, std::string> IdToValue; // defined in TimeTag.hpp
	using namespace gpstk;
%}


// Order matters - base classes should go before derived classes.
// If they don't, you will only get *WARNINGS* that base classes are used before they are declared.

typedef std::map< char, std::string> IdToValue;

%include "../../../src/TimeSystem.hpp"
%pythoncode %{ # TimeSystem.System enum replacement:
	def makeTimeSystem(system='Unknown'):
		return TimeSystem(TimeSystems[system])
	TimeSystems = {
         'Unknown' : 0,
         'Any'     : 1,
         'GPS'     : 2,
         'GLO'     : 3,
         'GAL'     : 4,
         'COM'     : 5,
         'UTC'     : 6,
         'UT1'     : 7,
         'TAI' 	   : 8,
         'TT'      : 9
    }
%}


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


%pythoncode %{
def str(self):
	return self.asString()

CommonTime.__str__ = str
UnixTime.__str__ = str
SystemTime.__str__ = str
ANSITime.__str__ = str
CivilTime.__str__ = str
GPSWeekSecond.__str__ = str
GPSWeekZcount.__str__ = str
JulianDate.__str__ = str
MJD.__str__ = str
YDSTime.__str__ = str
Exception.__str__ = str

%}
