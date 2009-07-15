//swig interface for RinexMetBase.hpp

%module gpstk_RinexMetBase

%{
#include "../../../src/FFData.hpp"
#include "../../../src/RinexMetBase.hpp"

using namespace gpstk;
%}

%include "../../../src/FFData.hpp"
%include "../../../src/RinexMetBase.hpp"
