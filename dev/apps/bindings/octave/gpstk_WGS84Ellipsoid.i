//swig interface for WGS84Ellipsoid.hpp

%module gpstk_WGS84Ellipsoid

%{
#include "../../../src/EllipsoidModel.hpp"
#include "../../../src/WGS84Ellipsoid.hpp"

using namespace gpstk;
%}
%include "../../../src/EllipsoidModel.hpp"
%include "../../../src/WGS84Ellipsoid.hpp"
