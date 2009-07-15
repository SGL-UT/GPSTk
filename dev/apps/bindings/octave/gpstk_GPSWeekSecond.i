//swig interface for GPSWeekSecond.hpp

%module gsptk_GPSWeekSecond
%{
#include "../../../src/GPSWeek.hpp"
#include "../../../src/GPSWeekSecond.hpp"

using namespace gpstk;
%}

%include "../../../src/GPSWeek.hpp"
%include "../../../src/GPSWeekSecond.hpp"