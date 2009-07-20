//swig interface for ObsClockModel.hpp

%module gpstk_ObsClockModel

%{
#include "../../../src/ClockModel.hpp"
#include "../../../src/ObsClockModel.hpp"

using namespace gpstk;
%}

%rename (ObsClockModel_streamRead) operator<<;
%include "../../../src/ClockModel.hpp"
%include "../../../src/ObsClockModel.hpp"
%rename (streamRead) operator<<;
