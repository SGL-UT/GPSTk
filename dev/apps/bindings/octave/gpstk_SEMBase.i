//swig interface for SEMBase.hpp

%module gpstk_SEMBase

%{
#include "../../../src/FFData.hpp"
#include "../../../src/SEMBase.hpp"

using namespace gpstk;
%}

%rename (SEMBase_streamRead) operator<<;
%rename (SEMBase_streamRead) operator>>;
%include "../../../src/FFData.hpp"
%include "../../../src/SEMBase.hpp"
%rename (streamRead) operator<<;
%rename (streamRead) operator>>;
