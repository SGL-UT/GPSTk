//swig interface for RTFileFrame.hpp

%module gpstk_RTFileFrame

%{
#include "../../../src/RTFileFrame.hpp"

using namespace gpstk;
%}

%rename (RTFileFrame_opequal) operator=;
%include "../../../src/RTFileFrame.hpp"
%rename (opequal) operator=;
