//swig interface for SystemTime.hpp

%module gpstk_SystemTime

%{
#include "../../../src/SystemTime.hpp"

using namespace gpstk;
%}

%include "../../../src/SystemTime.hpp"
