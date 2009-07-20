//swig interface for MJD.hpp"

%module gpstk_MJD

%{
#include "../../../src/TimeTag.hpp"
#include "../../../src/MJD.hpp"

using namespace gpstk;
%}

%rename (MJD_streamRead) operator<<;
%rename (FFData_CommonTime) operator CommonTime;
%include "../../../src/TimeTag.hpp"
%include "../../../src/MJD.hpp"
%rename (streamRead) operator<<;
%rename (CommonTime) operator CommonTime;
