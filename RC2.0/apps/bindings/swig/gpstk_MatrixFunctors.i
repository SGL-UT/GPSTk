//swig interface for MatrixFunctors.hpp

%module gpstk_MatrixFunctors

%{
#include "../../../src/MatrixFunctors.hpp"

using namespace gpstk;
%}

%include "../../../src/MatrixFunctors.hpp"