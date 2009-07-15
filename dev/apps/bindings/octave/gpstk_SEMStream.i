//swig interface for SEMStream.hpp

%module gpstk_SEMStream

%{
#include "../../../src/FFTextStream.hpp"
#include "../../../src/SEMStream.hpp"

using namespace gpstk;
%}

%include "../../../src/FFTextStream.hpp"
%include "../../../src/SEMStream.hpp"