//swig interface for RinexNavStream.hpp

%module gpstk_RinexNavStream

%{
#include "../../../src/RinexNavStream.hpp"

using namespace gpstk;
%}

%include "../../../src/RinexNavStream.hpp"
