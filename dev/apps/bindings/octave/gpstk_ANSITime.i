//swig interface for ANSITime.hpp

%module gpstk_ANSITime

%{
#include "../../../src/TimeTag.hpp"
#include "../../../src/ANSITime.hpp"

using namespace std;
using namespace gpstk;
%}

%include "../../../src/TimeTag.hpp"
%include "../../../src/ANSITime.hpp"
