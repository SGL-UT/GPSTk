//swig interface for GeoidModel.hpp

%module gpstk_GeoidModel

%{
#include "../../../src/GeoidModel.hpp"

using namespace gpstk;
%}

%include "../../../src/GeoidModel.hpp"
%include "gpstk_WGS84Geoid.i"
%include "gpstk_GPSGeoid.i"
