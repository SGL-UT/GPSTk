//swig interface for TypeID.hpp

%module gpstk_TypeID

%{
#include "../../../src/TypeID.hpp"

using namespace gpstk;
using namespace StringUtils;
%}

%include "../../../src/TypeID.hpp"