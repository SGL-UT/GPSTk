//swig interface for FICData9.hpp

%module gpstk_FICData9

%{
#include "../../../src/FICData.hpp"
#include "../../../src/FICData109.hpp"
#include "../../../src/FICData9.hpp"
#include "../../../src/FICData62.hpp"
#include "../../../src/FICData162.hpp"

using namespace gpstk;
%}

%rename (FICData_EngEphemeris) operator EngEphemeris;
%rename (FICData_AlmOrbit) operator AlmOrbit;
%include "../../../src/FICData.hpp"
%include "../../../src/FICData109.hpp"
%include "../../../src/FICData9.hpp"
%include "../../../src/FICData62.hpp"
%include "../../../src/FICData162.hpp"
%rename (EngEphemeris) operator EngEphemeris;
%rename (AlmOrbit) operator AlmOrbit;
