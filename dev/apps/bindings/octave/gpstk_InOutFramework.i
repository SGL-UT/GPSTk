//swig interface for InOutFramework.hpp

%module gpstk_InOutFramework

%{
#include "../../../src/LoopedFramework.hpp"
#include "../../../src/InOutFramework.hpp"

using namespace gpstk;
%}

%include "../../../src/LoopedFramework.hpp"
%include "../../../src/InOutFramework.hpp"
