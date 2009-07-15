//swig interface for MSCBase

%module gpstk_MSCBase

%{
#include "../../../src/FFData.hpp"
#include "../../../src/MSCBase.hpp"

using namespace gpstk;
%}

%include "../../../src/FFData.hpp"
%include "../../../src/MSCBase.hpp"