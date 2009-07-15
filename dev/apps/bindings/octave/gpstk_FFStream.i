//swig interface for FFStream.hpp

%module gpstk_FFStream

%{
#include "../../../src/FFStream.hpp"

using namespace std;
using namespace gpstk;
%}

%include "../../../src/FFStream.hpp"