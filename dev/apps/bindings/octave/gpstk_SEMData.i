//swig interface for SEMData.hpp

%module gpstk_SEMData

%{
#include "../../../src/SEMBase.hpp"
#include "../../../src/SEMData.hpp"

using namespace gpstk;
%}

%include "../../../src/SEMBase.hpp"
%include "../../../src/SEMData.hpp"