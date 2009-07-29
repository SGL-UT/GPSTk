//swig interface for Xvt.hpp

%module gpstk_Xvt

%{
#include "../../../src/Xvt.hpp"

using namespace gpstk;
%}

%rename (Xvt_streamRead) operator<<;
%include "../../../src/Xvt.hpp"
%rename (streamRead) operator<<;

