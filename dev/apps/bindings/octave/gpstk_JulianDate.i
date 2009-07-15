//swig interface for JulianDate.hpp

%module gpstk_JulianDate

%{
#include "../../../src/TimeTag.hpp"
#include "../../../src/JulianDate.hpp"

using namespace gpstk;
%}

%include "../../../src/TimeTag.hpp"
%include "../../../src/JulianDate.hpp"
