//swig interface for RinexEphemerisStore.hpp

%module gpstk_RinexEphemerisStore

%{
#include "../../../src/GPSEphemerisStore.hpp"
#include "../../../src/FileStore.hpp"
#include "../../../src/RinexEphemerisStore.hpp"

using namespace gpstk;
%}

%include "../../../src/GPSEphemerisStore.hpp"
%include "../../../src/FileStore.hpp"
%include "../../../src/RinexEphemerisStore.hpp"