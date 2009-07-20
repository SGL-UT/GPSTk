//swig interface for RinexMetBase.hpp

%module gpstk_RinexMetBase

%{
#include "../../../src/FFData.hpp"
#include "../../../src/RinexMetBase.hpp"

using namespace gpstk;
%}

%rename (RinexMetBase_streamRead) operator<<;
%rename (RinexMetBase_streamRead) operator>>;
%include "../../../src/FFData.hpp"
%include "../../../src/RinexMetBase.hpp"
%rename (streamRead) operator<<;
%rename (streamRead) operator>>;
