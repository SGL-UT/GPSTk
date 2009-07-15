//swig interface for YDSTime.hpp

%module gpstk_YDSTime

%{
#include "../../../src/TimeTag.hpp"
#include "../../../src/YDSTime.hpp"

using namespace gpstk;
%}

%include "../../../src/TimeTag.hpp"
%include "../../../src/YDSTime.hpp"