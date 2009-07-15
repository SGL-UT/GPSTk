//swig interface for TabularEphemerisStore.hpp

%module gpstk_TabularEphemerisStore

%{
#include "../../../src/XvtStore.hpp"
#include "../../../src/TabularEphemerisStore.hpp"

using namespace gpstk;
%}

%include "../../../src/XvtStore.hpp"
%include "../../../src/TabularEphemerisStore.hpp"