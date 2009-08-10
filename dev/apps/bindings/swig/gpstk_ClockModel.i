//swig interface for ClockModel.hpp

%module gpstk_ClockModel

%{
#include "../../../src/ClockModel.hpp"

using namespace gpstk;
%}

%rename (ClockModel_streamRead) operator<<;
%include "../../../src/ClockModel.hpp"
%include "gpstk_ObsClockModel.i"
%include "gpstk_LinearClockModel.i"
%rename (streamRead) operator<<;
