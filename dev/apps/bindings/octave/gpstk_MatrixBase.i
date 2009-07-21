//swig interface for MatrixBase.hpp

%module gpstk_MatrixBase

%{
#include "../../../src/MatrixBase.hpp"

using namespace gpstk;
%}

%rename (MatrixBase_plusequal) operator+=;
%rename (MatrixBase_minusequal) operator-=;
%rename (MatrixBase_starequal) operator*=;
%rename (MatrixBase_slashequal) operator/=;
%include "../../../src/MatrixBase.hpp"
