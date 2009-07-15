//swig interface for YumaStream.hpp

%module gpstk_YumaStream

%{
#include "../../../src/FFTextStream.hpp"
#include "../../../src/YumaStream.hpp"

using namespace gpstk;
%}

%include "../../../src/FFTextStream.hpp"
%include "../../../src/YumaStream.hpp"