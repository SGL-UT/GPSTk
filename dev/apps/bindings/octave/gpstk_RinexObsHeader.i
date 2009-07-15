//swig interface for RinexObsHeader.hpp

%module gpstk_RinexObsHeader

%{
#include "../../../src/RinexObsBase.hpp"
#include "../../../src/RinexObsHeader.hpp"

using namespace gpstk;
%}

%include "../../../src/RinexObsBase.hpp"
%include "../../../src/RinexObsHeader.hpp"
