//swig interface for TypeID.hpp

%module gpstk_TypeID

%{
#include "../../../src/TypeID.hpp"

using namespace gpstk;
//using namespace StringUtils;
%}
%ignore TypeIDsingleton;
%rename (TypeID_opequal) operator=;
%include "../../../src/TypeID.hpp"
%rename (opequal) operator=;



