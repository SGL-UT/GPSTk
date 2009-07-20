//swig interface for ClockModel.hpp

%module gpstk_ClockModel

%{
#include "../../../src/ClockModel.hpp"
#include "../../../src/ObsClockModel.hpp"
#include "../../../src/LinearClockModel.hpp"

using namespace gpstk;
%}

%rename (ClockModel_streamRead) operator<<;
%include "../../../src/ClockModel.hpp"
%include "../../../src/ObsClockModel.hpp"
%include "../../../src/LinearClockModel.hpp"
%rename (streamRead) operator<<;
