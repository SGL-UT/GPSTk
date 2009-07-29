//swig interface for FICStream.hpp

%module gpstk_FICStream

%{
#include "../../../src/FICStream.hpp"

using namespace gpstk;
%}

%include "../../../src/FICStream.hpp"
