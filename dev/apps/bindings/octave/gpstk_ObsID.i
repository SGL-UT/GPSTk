//swig interface for ObsID.hpp

%module gpstk_ObsID

%{
#include "../../../src/ObsID.hpp"

using namespace gpstk;
using namespace StringUtils;
%}

#include "../../../src/ObsID.hpp"