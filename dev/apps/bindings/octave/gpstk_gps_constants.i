//swig interface for gps_constants.hpp

%module gpstk_gps_constants

%{
#include "../../../src/gps_constants.hpp"

using namespace gpstk;
%}

%include "../../../src/gps_constants.hpp"