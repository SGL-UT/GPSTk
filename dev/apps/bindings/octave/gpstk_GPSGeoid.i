//swig interface for GPSGeoid.hpp

%module gpstk_GPSGeoid

%{
#include "../../../src/WGS84Geoid.hpp"
#include "../../../src/GPSGeoid.hpp"

using namespace gpstk;
%}

%include "../../../src/WGS84Geoid.hpp"
%include "../../../src/GPSGeoid.hpp"