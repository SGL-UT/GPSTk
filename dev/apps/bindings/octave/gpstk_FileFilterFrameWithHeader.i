//swig interface for FileFilterFrameWithHeader.hpp

%module gpstk_FileFilterFrameWithHeader

%{
#include "../../../src/FileFilterFrame.hpp"
#include "../../../src/FileFilterFrameWithHeader.hpp"

using namespace gpstk;
%}

%include "../../../src/FileFilterFrame.hpp"
%include "../../../src/FileFilterFrameWithHeader.hpp"