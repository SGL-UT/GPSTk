//swig interface for SEMData.hpp

%module gpstk_SEMData

%{
#include "../../../src/SEMBase.hpp"
#include "../../../src/SEMData.hpp"

using namespace gpstk;
%}

%rename (SEMData_AlmOrbit) operator AlmOrbit;
%include "../../../src/SEMBase.hpp"
%include "../../../src/SEMData.hpp"
%rename (AlmOrbit) operator AlmOrbit;
