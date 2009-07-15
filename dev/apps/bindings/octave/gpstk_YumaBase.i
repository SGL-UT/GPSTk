//swig interface for YumaBase.hpp

%module gpstk_YumaBase

%{
#include "../../../src/FFData.hpp"
#include "../../../src/YumaBase.hpp"

using namespace gpstk;
%}

%include "../../../src/FFData.hpp"
%include "../../../src/YumaBase.hpp"