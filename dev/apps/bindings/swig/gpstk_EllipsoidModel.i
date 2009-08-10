// swig interface for EllipsoidModel.hpp

%module gpstk_EllipsoidModel

%{
#include "../../../src/EllipsoidModel.hpp"

using namespace gpstk;
%}

%include "../../../src/EllipsoidModel.hpp"
%include "gpstk_WGS84Ellipsoid.i"
%include "gpstk_GPSEllipsoid.i"

