//swig interface for GPSWeek.hpp

%module gpstk_GPSWeek

%{
#include "../../../src/TimeTag.hpp"
#include "../../../src/GPSWeek.hpp"

using namespace gpstk;
%}

%include "../../../src/TimeTag.hpp"
%include "../../../src/GPSWeek.hpp"