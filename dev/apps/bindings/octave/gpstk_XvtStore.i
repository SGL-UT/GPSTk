//swig interface for XvtStore.hpp

%module gpstk_XvtStore

%{
#include "../../../src/XvtStore.hpp"

using namespace gpstk;
%}

%include "../../../src/XvtStore.hpp"
%include "gpstk_MSCStore.i"
%include "gpstk_GPSEphemerisStore.i"

