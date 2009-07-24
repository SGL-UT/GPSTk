//swig interface for GPSAlmanacStore.hpp

%module gpstk_GPSAlmanacStore

%{
//#include "../../../src/XvtStore.hpp"
#include "../../../src/GPSAlmanacStore.hpp"

using namespace gpstk;
%}

%template(SatID) gpstk::XvtStore<SatID>;
//%include "../../../src/XvtStore.hpp"
%include "../../../src/GPSAlmanacStore.hpp"
%include "gpstk_SEMAlmanacStore.i"
