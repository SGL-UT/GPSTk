//swig interface for RinexMetData.hpp

%module gpstk_RinexMetData

%{
#include "../../../src/RinexMetBase.hpp"
#include "../../../src/RinexMetData.hpp"

using namespace gpstk;
%}

%include "../../../src/RinexMetBase.hpp"
%include "../../../src/RinexMetData.hpp"