//swig interface for SP3EphemerisStore.hpp

%module gpstk_SP3EphemerisStore

%{
#include "../../../src/TabularEphemerisStore.hpp"
#include "../../../src/FileStore.hpp"
#include "../../../src/SP3EphemerisStore.hpp"

using namespace gpstk;
%}

%include "../../../src/TabularEphemerisStore.hpp"
%include "../../../src/FileStore.hpp"
%include "../../../src/SP3EphemerisStore.hpp"