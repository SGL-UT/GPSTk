//swig interface for ANSITime.hpp

%module gpstk_ANSITime

%{
#include "../../../src/TimeTag.hpp"
#include "../../../src/ANSITime.hpp"

using namespace std;
using namespace gpstk;
%}

%rename (ANSITime_streamRead) operator<<;
%include "../../../src/TimeTag.hpp"
%include "../../../src/ANSITime.hpp"
%rename (streamRead) operator<<;
