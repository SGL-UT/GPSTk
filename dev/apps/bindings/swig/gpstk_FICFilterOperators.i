//swig interface for FICFilterOperators.hpp

%module gpstk_FICFilterOperators

%{
#include "../../../src/FICFilterOperators.hpp"
using namespace std;
using namespace gpstk;
%}


//%template(FICDataFilterPRNblah)  std::unary_function<gpstk::FICData, bool>;

%include "../../../src/FICFilterOperators.hpp"
