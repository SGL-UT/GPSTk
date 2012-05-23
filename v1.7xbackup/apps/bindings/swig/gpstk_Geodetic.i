//swig interface for Geodetic.hpp

%module gpstk_Geodetic

%{
#include "../../../src/Triple.hpp"
#include "../../../src/Geodetic.hpp"

using namespace gpstk;
%}

%include "../../../src/Triple.hpp"
%include "../../../src/Geodetic.hpp"