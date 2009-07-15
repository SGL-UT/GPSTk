//swig interface for MatrixBase.hpp

%module gpstk_MatrixBase

%{
#include "../../../src/MatrixBase.hpp"

using namespace gpstk;
%}

%include "../../../src/MatrixBase.hpp"