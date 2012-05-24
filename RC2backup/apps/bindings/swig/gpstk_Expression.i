//swig interface for Expression.hpp

%module gpstk_Expression

%{
#include "../../../src/Expression.hpp"

using namespace gpstk;
%}

%include "../../../src/Expression.hpp"