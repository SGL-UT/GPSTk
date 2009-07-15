//swig interface for LoopedFramework.hpp

%module gpstk_LoopedFramework

%{
#include "../../../src/BasicFramework.hpp"
#include "../../../src/LoopedFramework.hpp"

using namespace gpstk;
%}

%include "../../../src/BasicFramework.hpp"
%include "../../../src/LoopedFramework.hpp"