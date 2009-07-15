//swig interface for SourceID.hpp

%module gpstk_SourceID

%{
#include "../../../src/SourceID.hpp"

using namespace gpstk;
using namespace StringUtils;
%}

%include "../../../src/SourceID.hpp"