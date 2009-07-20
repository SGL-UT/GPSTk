//swig interface for MSCBase

%module gpstk_MSCBase

%{
#include "../../../src/FFData.hpp"
#include "../../../src/MSCBase.hpp"

using namespace gpstk;
%}

%rename (MSCBase_streamRead) operator<<;
%rename (MSCBase_streamRead) operator>>;
%include "../../../src/FFData.hpp"
%include "../../../src/MSCBase.hpp"
%rename (streamRead) operator<<;
%rename (streamRead) operator>>;
