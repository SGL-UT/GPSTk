//swig interface for YumaStream.hpp

%module gpstk_YumaStream

%{
#include "../../../src/YumaStream.hpp"

using namespace gpstk;
%}

%include "../../../src/YumaStream.hpp"
