//swig interface for PCodeConst.hpp

%module PCodeConst

%{
#include "../../../src/PCodeConst.hpp"

using namespace gpstk;
%}

%include "../../../src/PCodeConst.hpp"