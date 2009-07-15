//swig interface for RinexNavBase.hpp

%module gpstk_RinexNavBase

%{
#include "../../../src/FFData.hpp"
#include "../../../src/RinexNavBase.hpp"

using namespace gpstk;
%}

%include "../../../src/FFData.hpp"
%include "../../../src/RinexNavBase.hpp"