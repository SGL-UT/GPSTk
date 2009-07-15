//swig interface for FICAStream.hpp

%module gpstk_FICAStream

%{
#include "../../../src/FICStreamBase.hpp"
#include "../../../src/FFTextStream.hpp"
#include "../../../src/FICAStream.hpp"

using namespace gpstk;
%}

%include "../../../src/FICStreamBase.hpp"
%include "../../../src/FFTextStream.hpp"
%include "../../../src/FICAStream.hpp"
