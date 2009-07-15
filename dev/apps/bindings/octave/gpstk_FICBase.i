//swig interface for FICBase.hpp

%module gpstk_FICBase

%{
#include "../../../src/FFData.hpp"
#include "../../../src/FICBase.hpp"

using namespace gpstk;
%}

%include "../../../src/FFData.hpp"
%include "../../../src/FICBase.hpp"