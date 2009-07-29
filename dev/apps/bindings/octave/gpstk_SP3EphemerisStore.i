//swig interface for SP3EphemerisStore.hpp

%module gpstk_SP3EphemerisStore

%{
#include "../../../src/TabularEphemerisStore.hpp"
#include "../../../src/SP3EphemerisStore.hpp"

using namespace gpstk;
%}

%template(FileStore_SP3Header) gpstk::FileStore< SP3Header >;
%include "../../../src/TabularEphemerisStore.hpp"
%include "../../../src/SP3EphemerisStore.hpp"
%template(FileStore_SP3Header) gpstk::FileStore< SP3Header >;
