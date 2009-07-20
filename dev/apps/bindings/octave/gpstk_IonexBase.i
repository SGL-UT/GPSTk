//swig interface for IonexBase.hpp

%module gpstk_IonexBase

%{
#include "../../../src/FFData.hpp"
#include "../../../src/IonexBase.hpp"

using namespace gpstk;
%}

%rename (IonexBase_streamRead) operator<<;
%rename (IonexBase_streamRead) operator>>;
%include "../../../src/FFData.hpp"
%include "../../../src/IonexBase.hpp"
%rename (streamRead) operator<<;
%rename (streamRead) operator>>;
