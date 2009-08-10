//swig interface for ObsID.hpp

%module gpstk_ObsID

%{
#include "../../../src/ObsID.hpp"

using namespace gpstk;
using namespace StringUtils;
%}

%ignore  singleton;
%include "../../../src/ObsID.hpp"
%include "gpstk_RinexObsID.i"



