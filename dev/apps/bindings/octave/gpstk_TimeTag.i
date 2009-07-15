//swig interface for TimeTag.hpp

%module gpstk_TimeTag

%{
#include "../../../src/TimeTag.hpp"

using namespace gpstk;
%}

%include "../../../src/TimeTag.hpp"