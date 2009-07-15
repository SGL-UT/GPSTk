//swig interface for SP3Base.hpp

%module gpstk_SP3Base

%{
#include "../../../src/FFData.hpp"
#include "../../../src/SP3Base.hpp"

using namespace gpstk;
%}

%include "../../../src/FFData.hpp"
%include "../../../src/SP3Base.hpp"
