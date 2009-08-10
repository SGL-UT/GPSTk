//swig interface for AlmOrbit.hpp

%module gpstk_AlmOrbit

%{
#include "../../../src/AlmOrbit.hpp"

using namespace std;
using namespace gpstk;
%}

%include "../../../src/AlmOrbit.hpp"
