//swig interface for gpstk_GPSEphemerisStore.hpp

%module gpstk_GPSEphemerisStore

%{
#include "../../../src/XvtStore.hpp"
#include "../../../src/GPSEphemerisStore.hpp"

using namespace gpstk;
%}

%include "../../../src/XvtStore.hpp"
%include "../../../src/GPSEphemerisStore.hpp"