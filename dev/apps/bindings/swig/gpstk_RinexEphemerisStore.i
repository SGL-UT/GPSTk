//swig interface for RinexEphemerisStore.hpp

%module gpstk_RinexEphemerisStore

%{
#include "../../../src/RinexEphemerisStore.hpp"

using namespace gpstk;
%}

%include "../../../src/RinexEphemerisStore.hpp"
