//swig interface for FileHunter.hpp

%module gpstk_FileHunter

%{
#include "../../../src/FileHunter.hpp"

using namespace gpstk;
%}

%include "../../../src/FileHunter.hpp"
