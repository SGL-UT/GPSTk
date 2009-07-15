//swig interface for RinexMetStream

%module gpstk_RinexMetStream

%{
#include "../../../src/FFTextStream.hpp"
#include "../../../src/RinexMetStream.hpp"

using namespace gpstk;
%}

%include "../../../src/FFTextStream.hpp"
%include "../../../src/RinexMetStream.hpp"