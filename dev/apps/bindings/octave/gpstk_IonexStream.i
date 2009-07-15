//swig interface for IonexStream.hpp

%module gpstk_IonexStream

%{
#include "../../../src/FFTextStream.hpp"
#include "../../../src/IonexStream.hpp"

using namespace gpstk;
%}

%include "../../../src/FFTextStream.hpp"
%include "../../../src/IonexStream.hpp"