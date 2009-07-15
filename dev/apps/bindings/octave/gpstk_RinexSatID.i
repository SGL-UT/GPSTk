//swig interface for RinexSatID.hpp

%module gpstk_RinexSatID

%{
#include "../../../src/SatID.hpp"
#include "../../../src/RinexSatID.hpp"

using namespace gpstk;
%}

%include "../../../src/SatID.hpp"
%include "../../../src/RinexSatID.hpp"
