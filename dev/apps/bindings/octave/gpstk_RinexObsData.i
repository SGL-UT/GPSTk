//swig interface for RinexObsData.hpp

%module gpstk_RinexObsData

%{
#include "../../../src/RinexObsBase.hpp"
#include "../../../src/RinexObsData.hpp"

using namespace gpstk;
%}

%include "../../../src/RinexObsBase.hpp"
%include "../../../src/RinexObsData.hpp"