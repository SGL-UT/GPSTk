//swig interface for SVExclusionList.hpp

%module gpstk_SVExclusionList

%{
#include "../../../src/SVExclusionList.hpp"

using namespace gpstk;
%}

%include "../../../src/SVExclusionList.hpp"

%{
typedef SVExclusionList::NoSVExclusionFound NoSVExclusionFound;
typedef SVExclusionList::SVExclusionFileNotFound SVExclusionFileNotFound;
%}
