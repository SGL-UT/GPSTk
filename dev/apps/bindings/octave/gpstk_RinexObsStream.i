//swig interface for RinexObsStream.hpp

%module gpstk_RinexObsStream

%{
#include "../../../src/FFTextStream.hpp"
#include "../../../src/RinexObsStream.hpp"

using namespace gpstk;
%}

%include "../../../src/FFTextStream.hpp"
%include "../../../src/RinexObsStream.hpp"