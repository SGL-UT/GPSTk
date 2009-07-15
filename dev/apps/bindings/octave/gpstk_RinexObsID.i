//swig interface for RinexObsID.hpp

%module gpstk_RinexObsID

%{
#include "../../../src/ObsID.hpp"
#include "../../../src/RinexObsID.hpp"

using namespace gpstk;
%}

%include "../../../src/ObsID.hpp"
%include "../../../src/RinexObsID.hpp"