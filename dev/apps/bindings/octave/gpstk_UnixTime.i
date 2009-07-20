//swig interface for UnixTime.hpp

%module gpstk_UnixTime

%{
#include "../../../src/TimeTag.hpp"
#include "../../../src/UnixTime.hpp"

using namespace gpstk;
%}

%rename (UnixTime_streamRead) operator<<;
%rename (UnixTime_CommonTime) operator CommonTime;
%include "../../../src/TimeTag.hpp"
%include "../../../src/UnixTime.hpp"
%rename (streamRead) operator<<;
%rename (CommonTime) operator CommonTime;
