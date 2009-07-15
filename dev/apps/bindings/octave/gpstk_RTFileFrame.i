//swig interface for RTFileFrame.hpp

%module gpstk_RTFileFrame

%{
#include "../../../src/FileSpec.hpp"
#include "../../../src/FileFilter.hpp"
#include "../../../src/RTFileFrame.hpp"

using namespace gpstk;
%}

%include "../../../src/FileSpec.hpp"
%include "../../../src/FileFilter.hpp"
%include "../../../src/RTFileFrame.hpp"