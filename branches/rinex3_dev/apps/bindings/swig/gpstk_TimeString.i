//swig interface for TimeString.hpp

%module gpstk_TimeString

%{
#include "../../../src/TimeString.hpp"

using namespace gpstk;
%}

%include "../../../src/TimeString.hpp"