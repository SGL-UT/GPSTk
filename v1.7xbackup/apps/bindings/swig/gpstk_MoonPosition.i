//swig interface for MoonPosition.hpp

%module gpstk_MoonPosition

%{
#include "../../../src/MoonPosition.hpp"

using namespace gpstk;
%}

%include "../../../src/MoonPosition.hpp"