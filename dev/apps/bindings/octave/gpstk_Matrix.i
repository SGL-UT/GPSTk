//swig interface for Matrix.hpp

%module gpstk_Matrix
%{
#include "../../../src/Matrix.hpp"

using namespace gpstk;
%}

%rename (Matrix_opequal) operator=;
%include "../../../src/Matrix.hpp"
%include "gpstk_MatrixBase.i"
%include "gpstk_PRSolution.i"


