//swig interface for YumaBase.hpp

%module gpstk_YumaBase

%{
#include "../../../src/FFData.hpp"
#include "../../../src/YumaBase.hpp"
#include "../../../src/YumaHeader.hpp"

using namespace gpstk;
%}

%rename (YumaBase_streamRead) operator<<;
%rename (YumaBase_streamRead) operator>>;
%include "../../../src/FFData.hpp"
%include "../../../src/YumaBase.hpp"
%include "../../../src/YumaHeader.hpp"
%rename (streamRead) operator<<;
%rename (streamRead) operator>>;
