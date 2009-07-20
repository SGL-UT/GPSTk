//swig interface for SP3Base.hpp

%module gpstk_SP3Base

%{
#include "../../../src/FFData.hpp"
#include "../../../src/SP3Base.hpp"

using namespace gpstk;
%}

%rename (SP3Base_streamRead) operator<<;
%rename (SP3Base_streamRead) operator>>;
%include "../../../src/FFData.hpp"
%include "../../../src/SP3Base.hpp"
%rename (streamRead) operator<<;
%rename (streamRead) operator>>;
