//swig interface for WxObsMap.hpp

%module gpstk_WxObsMap

%{
#include "../../../src/WxObsMap.hpp"

using namespace gpstk;
%}

%rename (WxObsMap_streamRead) operator<<;
%include "../../../src/WxObsMap.hpp"
%rename (streamRead) operator<<;
