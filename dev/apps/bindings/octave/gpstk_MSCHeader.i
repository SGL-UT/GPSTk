//swig interface for MSCHeader

%module gpstk_MSCHeader

%{
#include "../../../src/MSCBase.hpp"
#include "../../../src/MSCHeader.hpp"

using namespace gpstk;
%}

%include "../../../src/MSCBase.hpp"
%include "../../../src/MSCHeader.hpp"