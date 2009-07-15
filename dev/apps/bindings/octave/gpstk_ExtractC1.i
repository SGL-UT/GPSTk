//swig interface for ExtractC1.hpp

%module gpstk_ExtractC1

%{
#include "../../../src/ExtractData.hpp"
#include "../../../src/ExtractC1.hpp"

using namespace gpstk;
%}

%include "../../../src/ExtractData.hpp"
%include "../../../src/ExtractC1.hpp"
