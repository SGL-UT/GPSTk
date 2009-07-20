//swig interface for ORDEpoch.hpp

%module gpstk_ORDEpoch

%{
#include "../../../src/ORDEpoch.hpp"

using namespace gpstk;
%}

%rename (ORDEpoch_streamRead) operator<<;
%include "../../../src/ORDEpoch.hpp"
%rename (streamRead) operator<<;
