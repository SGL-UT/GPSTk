//swig interface for TimeTag.hpp

%module gpstk_TimeTag

%{
#include "../../../src/TimeTag.hpp"

using namespace gpstk;
%}
%rename (GPSWeek_opequal) operator=;
%rename (TimeTag_streamRead) operator<<;
%rename (TimeTag_CommonTime) operator CommonTime;
%include "../../../src/TimeTag.hpp"
//%include "gpstk_CivilTime.i"
%include "gpstk_ANSITime.i"
%include "gpstk_GPSWeek.i"
%include "gpstk_GPSWeekSecond.i"
%include "gpstk_GPSWeekZcount.i"
%include "gpstk_JulianDate.i"
%include "gpstk_MJD.i"
%include "gpstk_UnixTime.i"
%include "gpstk_SystemTime.i"
%include "gpstk_YDSTime.i"
%rename (opequal) operator=;
%rename (streamRead) operator<<;
%rename (CommonTime) operator CommonTime;

