// SWIG interface for the gpstk_time module

%include "std_string.i"

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
%include "../../../src/TimeTag.hpp"
%include "../../../src/TimeConstants.hpp"

%include "../../../src/CommonTime.hpp"
%extend gpstk::CommonTime {
	std::string __str__() {
		return $self->asString();
	}
};

%feature("notabstract") UnixTime;
%include "../../../src/UnixTime.hpp"
%extend gpstk::UnixTime {
	std::string __str__() {
		return $self->asString();
	}
};

%feature("notabstract") SystemTime;
%include "../../../src/SystemTime.hpp"
%extend gpstk::SystemTime {
	std::string __str__() {
		return $self->asString();
	}
};

%feature("notabstract") ANSITime;
%include "../../../src/ANSITime.hpp"
%extend gpstk::ANSITime {
	std::string __str__() {
		return $self->asString();
	}
};

%feature("notabstract") CivilTime;
%include "../../../src/CivilTime.hpp"
%extend gpstk::CivilTime {
	std::string __str__() {
		return $self->asString();
	}
};

%include "../../../src/GPSZcount.hpp"
%include "../../../src/GPSWeek.hpp"

%feature("notabstract") GPSWeekSecond;
%include "../../../src/GPSWeekSecond.hpp"
%extend gpstk::GPSWeekSecond {
	std::string __str__() {
		return $self->asString();
	}
};

%feature("notabstract") GPSWeekZcount;
%include "../../../src/GPSWeekZcount.hpp"
%extend gpstk::GPSWeekZcount {
	std::string __str__() {
		return $self->asString();
	}
};

%feature("notabstract") JulianDate; 
%include "../../../src/JulianDate.hpp"
%extend gpstk::JulianDate {
	std::string __str__() {
		return $self->asString();
	}
};

%feature("notabstract") MJD;
%include "../../../src/MJD.hpp"
%extend gpstk::MJD {
	std::string __str__() {
		return $self->asString();
	}
};

%feature("notabstract") YDSTime;
%include "../../../src/YDSTime.hpp"
%extend gpstk::YDSTime {
	std::string __str__() {
		return $self->asString();
	}
};

%include "../../../src/TimeConverters.hpp"
%include "../../../src/TimeString.hpp"

%include "../../../src/Exception.hpp"
%extend gpstk::Exception {
	std::string __str__() {
		return $self->getText();
	}
};