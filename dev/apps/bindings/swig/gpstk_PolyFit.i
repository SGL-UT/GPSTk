//swig interface for Polyfit.hpp

%module gpstk_PolyFit

%{
#include "../../../src/PolyFit.hpp"

using namespace gpstk;
%}

%include "../../../src/PolyFit.hpp"