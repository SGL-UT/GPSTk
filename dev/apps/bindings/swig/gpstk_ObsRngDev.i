//swig interface for ObsRngDev.hpp

%module gpstk_ObsRngDev

%{
#include "../../../src/ObsRngDev.hpp"

using namespace gpstk;
%}

%rename (ObsRngDev_streamRead) operator<<;
%include "../../../src/ObsRngDev.hpp"
%rename (streamRead) operator<<;
