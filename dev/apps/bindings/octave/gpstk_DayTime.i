//swig interface for DayTime.hpp

%module gpstk_DayTime

%{
#include "../../../src/DayTime.hpp"

using namespace gpstk;
%}

%include "../../../src/DayTime.hpp"

%{
typedef DayTime::DayTimeException DayTimeException;
typedef DayTime::FormatException FormatException;
%}
