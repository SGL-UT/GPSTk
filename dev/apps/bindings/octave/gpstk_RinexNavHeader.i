//swig interface for RinexNavHeader.hpp

%module gpstk_RinexNavHeader

%{
#include "../../../src/RinexNavBase.hpp"
#include "../../../src/RinexNavHeader.hpp"

using namespace gpstk;
%}

%include "../../../src/RinexNavBase.hpp"
%include "../../../src/RinexNavHeader.hpp"

