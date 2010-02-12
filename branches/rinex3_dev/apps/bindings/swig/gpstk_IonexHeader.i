//swig interface for IonexHeader.hpp

%module gpstk_IonexHeader

%{
#include "../../../src/IonexHeader.hpp"

using namespace gpstk;
%}

%include "../../../src/IonexHeader.hpp"

%{
typedef IonexHeader::DCB DCB;
%}
