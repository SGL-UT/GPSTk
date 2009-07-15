//swig interface for SP3Stream.hpp

%module gpstk_SP3Stream

%{
#include "../../../src/FFTextStream.hpp"
#include "../../../src/SP3Stream.hpp"

using namespace gpstk;
%}

%include "../../../src/FFTextStream.hpp"
%include "../../../src/SP3Stream.hpp"