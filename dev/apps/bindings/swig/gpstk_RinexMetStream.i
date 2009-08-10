//swig interface for RinexMetStream

%module gpstk_RinexMetStream

%{
#include "../../../src/RinexMetStream.hpp"

using namespace gpstk;
%}

%include "../../../src/RinexMetStream.hpp"
