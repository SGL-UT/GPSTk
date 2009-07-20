//swig interface for TimeTag.hpp

%module gpstk_TimeTag

%{
#include "../../../src/TimeTag.hpp"
#include "../../../src/GPSWeek.hpp"
#include "../../../src/GPSWeekSecond.hpp"
#include "../../../src/GPSWeekZcount.hpp"

using namespace gpstk;
%}

%rename (TimeTag_streamRead) operator<<;
%rename (TimeTag_CommonTime) operator CommonTime;
%include "../../../src/TimeTag.hpp"
%include "../../../src/GPSWeek.hpp"
%include "../../../src/GPSWeekSecond.hpp"
%include "../../../src/GPSWeekZcount.hpp"
%rename (streamRead) operator<<;
%rename (CommonTime) operator CommonTime;
