//swig interface for gpstk_GPSEphemerisStore.hpp

%module gpstk_GPSEphemerisStore

%{
#include "../../../src/GPSEphemerisStore.hpp"

using namespace gpstk;
%}

%include "../../../src/GPSEphemerisStore.hpp"
%include "gpstk_RinexEphemerisStore.i"
