//swig interface for GPSEllipsiod.hpp

%module gpstk_GPSEllipsoid

%{
#include "../../../src/GPSEllipsoid.hpp"

using namespace gpstk;
%}

%include "../../../src/GPSEllipsoid.hpp"
