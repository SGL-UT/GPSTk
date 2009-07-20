//swig interface for YumaData.hpp

%module gpstk_YumaData

%{
#include "../../../src/YumaBase.hpp"
#include "../../../src/YumaData.hpp"

using namespace gpstk;
%}

%rename (YumaData_AlmOrbit) operator AlmOrbit;
%include "../../../src/YumaBase.hpp"
%include "../../../src/YumaData.hpp"
%rename (AlmOrbit) operator AlmOrbit;
