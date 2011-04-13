//swig interface for PRSolution.hpp

%module gpstk_PRSolution

%{
#include "../../../src/PRSolution.hpp"

using namespace gpstk;
%}

%include "../../../src/PRSolution.hpp"