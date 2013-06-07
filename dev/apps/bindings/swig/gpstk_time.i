//swig interface for the gpstk_time module

%module gpstk_time

%{
#include "../../../src/TimeSystem.hpp"
#include "../../../src/TimeTag.hpp"
#include "../../../src/UnixTime.hpp"
#include "../../../src/ANSITime.hpp"
#include "../../../src/CivilTime.hpp"
#include "../../../src/CommonTime.hpp"
#include "../../../src/Exception.hpp"
#include "../../../src/GPSZcount.hpp"
#include "../../../src/GPSWeek.hpp"
#include "../../../src/GPSWeekSecond.hpp"
#include "../../../src/GPSWeekZcount.hpp"
#include "../../../src/JulianDate.hpp"
#include "../../../src/MJD.hpp"
#include "../../../src/StringUtils.hpp"
#include "../../../src/SystemTime.hpp"
#include "../../../src/TimeConstants.hpp"
#include "../../../src/TimeConverters.hpp"
#include "../../../src/TimeString.hpp"
#include "../../../src/YDSTime.hpp"
typedef std::map< char, std::string> IdToValue; // defined in TimeTag.hpp
using namespace gpstk;
%}


// Order matters! Base classes should go before derived classes.
// If they don't, you will get WARNINGS that base classes are used before they are declared.

%include "std_string.i"
typedef std::map< char, std::string> IdToValue;
%template(StrVector) vector<std::string>;

%include "../../../src/TimeSystem.hpp"
%include "../../../src/TimeTag.hpp"
%include "../../../src/UnixTime.hpp"
%include "../../../src/ANSITime.hpp"
%include "../../../src/CivilTime.hpp"
%include "../../../src/CommonTime.hpp"
%include "../../../src/GPSZcount.hpp"
%include "../../../src/GPSWeek.hpp"

%feature("notabstract") GPSWeekSecond;  // this may be a bad idea...
%include "../../../src/GPSWeekSecond.hpp"

%feature("notabstract") GPSWeekZcount;
%include "../../../src/GPSWeekZcount.hpp"

%feature("notabstract") JulianDate; 
%include "../../../src/JulianDate.hpp"

%feature("notabstract") MJD;
%include "../../../src/MJD.hpp"

%feature("notabstract") YDSTime;
%include "../../../src/YDSTime.hpp"

%include "../../../src/SystemTime.hpp"
%include "../../../src/TimeConstants.hpp"
%include "../../../src/TimeConverters.hpp"
%include "../../../src/TimeString.hpp"