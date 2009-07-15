//swig interface for GPSEllipsiod.hpp

%module gpstk_GPSEllipsoid

%{
#include "../../../src/WGS84Ellipsoid.hpp"
#include "../../../src/GPSEllipsoid.hpp"

using namespace gpstk;
%}

%include "../../../src/WGS84Ellipsoid.hpp"
%include "../../../src/GPSEllipsoid.hpp"
