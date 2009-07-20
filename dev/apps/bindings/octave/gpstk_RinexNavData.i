//swig interface for RinexNavData.hpp

%module gpstk_RinexNaveData

%{
#include "../../../src/RinexNavBase.hpp"
#include "../../../src/RinexNavData.hpp"

using namespace gpstk;
%}

%rename (RinexNavData_EngEphemeris) operator EngEphemeris;
%include "../../../src/RinexNavBase.hpp"
%include "../../../src/RinexNavData.hpp"
%rename (EngEphemeris) operator EngEphemeris;
