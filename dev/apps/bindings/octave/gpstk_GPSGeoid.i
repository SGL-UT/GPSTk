//swig interface for GPSGeoid.hpp

%module gpstk_GPSGeoid

%{
#include "../../../src/GeoidModel.hpp"
#include "../../../src/WGS84Geoid.hpp"
#include "../../../src/GPSGeoid.hpp"

using namespace gpstk;
%}

%include "../../../src/GeoidModel.hpp"
%include "../../../src/WGS84Geoid.hpp"
%include "../../../src/GPSGeoid.hpp"
