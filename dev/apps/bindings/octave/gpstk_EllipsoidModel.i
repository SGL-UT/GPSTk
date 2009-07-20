// swig interface for EllipsoidModel.hpp

%module gpstk_EllipsoidModel

%{
#include "../../../src/EllipsoidModel.hpp"
#include "../../../src/WGS84Ellipsoid.hpp"
#include "../../../src/GPSEllipsoid.hpp"


using namespace gpstk;
%}

%include "../../../src/EllipsoidModel.hpp"
%include "../../../src/WGS84Ellipsoid.hpp"
%include "../../../src/GPSEllipsoid.hpp"

