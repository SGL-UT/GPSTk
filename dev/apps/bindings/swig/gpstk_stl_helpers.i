//swig interface for stl_helpers.hpp

%module gpstk_stl_helpers

%{
#include "../../../src/stl_helpers.hpp"

using namespace gpstk;
%}

%include "../../../src/stl_helpers.hpp"