//swig interface for VectorBaseOperators

%module gpstk_VectorBaseOperators

%{
#include "../../../src/VectorOperators.hpp"
#include "../../../src/VectorBaseOperators.hpp"

using namespace gpstk;
%}

%include "../../../src/VectorOperators.hpp"
%include "../../../src/VectorBaseOperators.hpp"

