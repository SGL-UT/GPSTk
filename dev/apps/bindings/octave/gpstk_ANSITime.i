//swig interface for ANSITime.hpp

%module gpstk_ANSITime

%{
#include "../../../src/TimeTag.hpp"
#include "../../../src/ANSITime.hpp"

using namespace std;
using namespace gpstk;
%}

%rename (ANSITime_opequal) operator=;
%rename (ANSITime_streamRead) operator<<;
%rename (ANSITime_CommonTime) operator CommonTime;
%include "../../../src/TimeTag.hpp"
%include "../../../src/ANSITime.hpp"

