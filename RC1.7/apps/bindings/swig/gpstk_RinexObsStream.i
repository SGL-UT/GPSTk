//swig interface for RinexObsStream.hpp

%module gpstk_RinexObsStream

%{
#include "../../../src/RinexObsStream.hpp"

using namespace gpstk;
%}

%include "../../../src/RinexObsStream.hpp"
