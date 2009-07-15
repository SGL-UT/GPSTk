//swig interface for SEMBase.hpp

%module gpstk_SEMBase

%{
#include "../../../src/FFData.hpp"
#include "../../../src/SEMBase.hpp"

using namespace gpstk;
%}

%include "../../../src/FFData.hpp"
%include "../../../src/SEMBase.hpp"