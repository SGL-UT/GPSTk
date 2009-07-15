//swig interface for ObsClockModel.hpp

%module gpstk_ObsClockModel

%{
#include "../../../src/ClockModel.hpp"
#include "../../../src/ObsClockModel.hpp"

using namespace gpstk;
%}

%include "../../../src/ClockModel.hpp"
%include "../../../src/ObsClockModel.hpp"