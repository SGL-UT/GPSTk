//swig interface for DayTime.hpp

%module gpstk_DayTime

%{
#include "../../../src/DayTime.hpp"

using namespace gpstk;
%}

%rename (DayTime_CommonTime) operator CommonTime;
%rename (DayTime_GPSZcount) operator GPSZcount;
%include "../../../src/DayTime.hpp"
%rename (CommonTime) operator CommonTime;
%rename (GPSZcount) operator GPSZcount;

%{
typedef DayTime::DayTimeException DayTimeException;
typedef DayTime::FormatException FormatException;
%}
