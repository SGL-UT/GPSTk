//swig interface for RinexNavBase.hpp

%module gpstk_RinexNavBase

%{
#include "../../../src/FFData.hpp"
#include "../../../src/RinexNavBase.hpp"

using namespace gpstk;
%}

%rename (RinexNavBase_streamRead) operator<<;
%rename (RinexNavBase_streamRead) operator>>;
%include "../../../src/FFData.hpp"
%include "../../../src/RinexNavBase.hpp"
%rename (streamRead) operator<<;
%rename (streamRead) operator>>;
