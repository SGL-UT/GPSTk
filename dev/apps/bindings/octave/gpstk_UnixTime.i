//swig interface for UnixTime.hpp

%module gpstk_UnixTime

%{
#include "../../../src/TimeTag.hpp"
#include "../../../src/UnixTime.hpp"

using namespace gpstk;
%}

%include "../../../src/TimeTag.hpp"
%include "../../../src/UnixTime.hpp"