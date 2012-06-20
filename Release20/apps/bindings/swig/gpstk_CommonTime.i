//swig interface for CommonTime.hpp

%module gpstk_CommonTime

%{
#include "../../../src/CommonTime.hpp"

using namespace gpstk;
%}

%rename (CommonTime_opequal) operator=;
%rename (CommonTime_plusequal) operator+=;
%rename (CommonTime_minusequal) operator-=;
%include "../../../src/CommonTime.hpp"
%rename (opequal) operator=;
%rename (plusequal) operator+=;
%rename (minusequal) operator-=;
