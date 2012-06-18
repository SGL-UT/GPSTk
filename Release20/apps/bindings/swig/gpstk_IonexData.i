//swig interface for IonexData.hpp

%module gpstk_IonexData

%{
#include "../../../src/IonexData.hpp"

using namespace gpstk;
%}

%include "../../../src/IonexData.hpp"

%{
typedef IonexData::IonexValType IonexValType;
%}
