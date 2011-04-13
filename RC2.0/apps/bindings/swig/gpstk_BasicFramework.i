//swig interface for BasicFramework.hpp

%module gpstk_BasicFramework

%{
#include "../../../src/BasicFramework.hpp"

using namespace std;
using namespace gpstk;
%}

%include "../../../src/BasicFramework.hpp"
%include "gpstk_LoopedFramework.i"
%include "gpstk_InOutFramework.i"
