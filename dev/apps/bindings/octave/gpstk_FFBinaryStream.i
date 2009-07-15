//swig interface for FFBinaryStream

%module gpstk_FFBinaryStream

%{
#include "../../../src/FFStream.hpp"
#include "../../../src/FFBinaryStream.hpp"

using namespace gpstk;
%}

%include "../../../src/FFStream.hpp"
%include "../../../src/FFBinaryStream.hpp"