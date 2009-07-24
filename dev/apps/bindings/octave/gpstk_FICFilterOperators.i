//swig interface for FICFilterOperators.hpp

%module gpstk_FICFilterOperators

%{
#include "../../../src/FICFilterOperators.hpp"

using namespace gpstk;
%}

%template (FICData) std::unary_function< gpstk::FICData,bool >;
%template (FICData) std::binary_function< gpstk::FICData,gpstk::FICData,bool >;
%include "../../../src/FICFilterOperators.hpp"
