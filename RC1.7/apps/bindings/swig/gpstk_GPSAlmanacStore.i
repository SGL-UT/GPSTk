//swig interface for GPSAlmanacStore.hpp

%module gpstk_GPSAlmanacStore

%{

#include "../../../src/GPSAlmanacStore.hpp"

using namespace gpstk;
%}

%include "../../../src/GPSAlmanacStore.hpp"
%include "gpstk_SEMAlmanacStore.i"
%include "gpstk_YumaAlmanacStore.i"
