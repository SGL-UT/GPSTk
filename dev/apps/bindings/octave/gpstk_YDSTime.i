//swig interface for YDSTime.hpp

%module gpstk_YDSTime

%{
#include "../../../src/TimeTag.hpp"
#include "../../../src/YDSTime.hpp"

using namespace gpstk;
%}

%rename (YDSTime_streamRead) operator<<;
%rename (YDSTime_CommonTime) operator CommonTime;
%include "../../../src/TimeTag.hpp"
%include "../../../src/YDSTime.hpp"
%rename (streamRead) operator<<;
%rename (CommonTime) operator CommonTime;
