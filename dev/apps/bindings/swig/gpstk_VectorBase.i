//swig interface for VectorBase.hpp

%module gpstk_VectorBase

%{
#include "../../../src/VectorBase.hpp"

using namespace gpstk;
%}

%include "../../../src/VectorBase.hpp"