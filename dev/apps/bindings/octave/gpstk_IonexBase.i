//swig interface for IonexBase.hpp

%module gpstk_IonexBase

%{
#include "../../../src/FFData.hpp"
#include "../../../src/IonexBase.hpp"

using namespace gpstk;
%}

%include "../../../src/FFData.hpp"
%include "../../../src/IonexBase.hpp"