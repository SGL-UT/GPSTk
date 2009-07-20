//swig interface for TimeTag.hpp

%module gpstk_TimeTag

%{
#include "../../../src/TimeTag.hpp"

using namespace gpstk;
%}

%rename (TimeTag_streamRead) operator<<;
%rename (TimeTag_CommonTime) operator CommonTime;
%include "../../../src/TimeTag.hpp"
%rename (streamRead) operator<<;
%rename (CommonTime) operator CommonTime;
