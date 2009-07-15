//swig interface for RinexObsBase.hpp

%module gpstk_RinexObsBase

%{
#include "../../../src/FFData.hpp"
#include "../../../src/RinexObsBase.hpp"

using namespace gpstk;
%}

%include "../../../src/FFData.hpp"
%include "../../../src/RinexObsBase.hpp"