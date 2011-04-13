//swig interface for DayTime.hpp

%module gpstk_DayTime

 // jay had to add:
struct timeval
  {
    int tv_sec;            /* Seconds.  */
    int tv_usec;           /* Microseconds.  */
  };

%{
#include "../../../src/DayTime.hpp"

using namespace gpstk;
%}

%rename (DayTime_opequal) operator=;
%rename (DayTime_plusequal) operator+=;
%rename (DayTime_minusequal) operator-=;
%rename (DayTime_CommonTime) operator CommonTime;
%rename (DayTime_GPSZcount) operator GPSZcount;
%include "../../../src/DayTime.hpp"
%rename (opequal) operator=;
%rename (plusequal) operator+=;
%rename (minusequal) operator-=;
%rename (CommonTime) operator CommonTime;
%rename (GPSZcount) operator GPSZcount;
%{
typedef DayTime::DayTimeException DayTimeException;
typedef DayTime::FormatException FormatException;
%}
