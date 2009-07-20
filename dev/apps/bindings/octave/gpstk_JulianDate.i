//swig interface for JulianDate.hpp

%module gpstk_JulianDate

%{
#include "../../../src/TimeTag.hpp"
#include "../../../src/JulianDate.hpp"

using namespace gpstk;
%}

%rename (JulianDate_streamRead) operator<<;
%rename (Triple_CommonTime) operator CommonTime;
%include "../../../src/TimeTag.hpp"
%include "../../../src/JulianDate.hpp"
%rename (streamRead) operator<<;
%rename (CommonTime) operator CommonTime;

