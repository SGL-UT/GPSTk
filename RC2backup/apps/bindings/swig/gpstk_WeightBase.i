//swig interface for WeightBase.hpp

%module gpstk_WeightBase

%{
#include "../../../src/WeightBase.hpp"

using namespace gpstk;
%}

%include "../../../src/WeightBase.hpp"
%include "gpstk_SimpleIURAWeight.i"
