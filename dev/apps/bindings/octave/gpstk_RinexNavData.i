//swig interface for RinexNavData.hpp

%module gpstk_RinexNaveData

%{
#include "../../../src/RinexNavBase.hpp"
#include "../../../src/RinexNavData.hpp"

using namespace gpstk;
%}

%include "../../../src/RinexNavBase.hpp"
%include "../../../src/RinexNavData.hpp"