//swig interface for FFTextStream

%module gpstk_FFTextStream

%{
#include "../../../src/FFStream.hpp"
#include "../../../src/FFTextStream.hpp"

using namespace gpstk;
%}

%include "../../../src/FFStream.hpp"
%include "../../../src/FFTextStream.hpp"