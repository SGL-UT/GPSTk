//swig interface for SatID.hpp

%module gpstk_SatID

%{
#include "../../../src/SatID.hpp"

using namespace gpstk;
%}

%include "../../../src/SatID.hpp"
%include "gpstk_RinexSatID.i"
%include "gpstk_SP3SatID.i"
