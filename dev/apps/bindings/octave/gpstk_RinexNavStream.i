//swig interface for RinexNavStream.hpp

%module gpstk_RinexNavStream

%{
#include "../../../src/FFTextStream.hpp"
#include "../../../src/RinexNavStream.hpp"

using namespace gpstk;
%}

%include "../../../src/FFTextStream.hpp"
%include "../../../src/RinexNavStream.hpp"
