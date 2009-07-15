//swig interface for MSCStream.hpp

%module gpstk_MSCStream

%{
#include "../../../src/FFTextStream.hpp"
#include "../../../src/MSCStream.hpp"

using namespace gpstk;
%}

%include "../../../src/FFTextStream.hpp"
%include "../../../src/MSCStream.hpp"