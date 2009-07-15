//swig interface for SimpleIURAWeight.hpp

%module gpstk_SimpleIURAWeight

%{
#include "../../../src/WeightBase.hpp"
#include "../../../src/SimpleIURAWeight.hpp"

using namespace gpstk;
%}

%include "../../../src/WeightBase.hpp"
%include "../../../src/SimpleIURAWeight.hpp"