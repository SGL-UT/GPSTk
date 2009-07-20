//swig interface for LinearClockModel.hpp

%module gpstk_LinearClockModel

%{
#include "../../../src/ObsClockModel.hpp"
#include "../../../src/LinearClockModel.hpp"

using namespace gpstk;
%}

%rename (LinearClockModel_streamRead) operator<<;
%include "../../../src/ObsClockModel.hpp"
%include "../../../src/LinearClockModel.hpp"
%rename (streamRead) operator<<;
