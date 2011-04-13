//swig interface for FICAStream.hpp

%module gpstk_FICAStream

%{
#include "../../../src/FICAStream.hpp"

using namespace gpstk;
%}

%include "../../../src/FICAStream.hpp"
