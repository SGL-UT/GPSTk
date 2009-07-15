//swig interface for FFData.hpp

%module gpstk_FFData

%{
#include "../../../src/FFData.hpp"

using namespace gpstk;
%}

%include "../../../src/FFData.hpp"