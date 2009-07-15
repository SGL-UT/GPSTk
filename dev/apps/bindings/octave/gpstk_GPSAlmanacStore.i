//swig interface for GPSAlmanacStore.hpp

%module gpstk_GPSAlmanacStore

%{
#include "../../../src/XvtStore.hpp"
#include "../../../src/GPSAlmanacStore.hpp"

using namespace gpstk;
%}

%include "../../../src/XvtStore.hpp"
%include "../../../src/GPSAlmanacStore.hpp"
