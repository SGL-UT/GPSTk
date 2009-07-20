//swig interface for GPSWeek.hpp

%module gpstk_GPSWeek

%{
#include "../../../src/TimeTag.hpp"
#include "../../../src/GPSWeek.hpp"

using namespace gpstk;
%}

%rename (GPSWeek_streamRead) operator<<;
%rename (GPSWeek_CommonTime) operator CommonTime;
%include "../../../src/TimeTag.hpp"
%include "../../../src/GPSWeek.hpp"
%rename (streamRead) operator<<;
%rename (CommonTime) operator CommonTime;
