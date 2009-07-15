//swig interface for IonexData.hpp

%module gpstk_IonexData

%{
#include "../../../src/IonexBase.hpp"
#include "../../../src/IonexData.hpp"

using namespace gpstk;
%}

%include "../../../src/IonexBase.hpp"
%include "../../../src/IonexData.hpp"

%{
typedef IonexData::IonexValType IonexValType;
%}
