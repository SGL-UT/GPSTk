//swig interface foe WGS84Geoid.hpp

%module gpstk_WGS84Geoid

%{
#include "../../../src/GeoidModel.hpp"
#include "../../../src/WGS84Geoid.hpp"

using namespace gpstk;
%}

%include "../../../src/GeoidModel.hpp"
%include "../../../src/WGS84Geoid.hpp"