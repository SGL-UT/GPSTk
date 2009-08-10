//swig interface for MatrixImplementation.hpp

%module gpstk_MatrixImplementation

%{
#include "../../../src/MatrixImplementation.hpp"

using namespace gpstk;
%}

%include "../../../src/MatrixImplementation.hpp"