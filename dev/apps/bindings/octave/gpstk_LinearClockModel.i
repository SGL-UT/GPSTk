//swig interface for LinearClockModel.hpp

%module gpstk_LinearClockModel

%{
#include "../../../src/ObsClockModel.hpp"
#include "../../../src/LinearClockModel.hpp"

using namespace gpstk;
%}

%include "../../../src/ObsClockModel.hpp"
%include "../../../src/LinearClockModel.hpp"