//swig interface for BivarStats.hpp

%module gpstk_BivarStats

%{
#include "../../../src/BivarStats.hpp"

using namespace std;
using namespace gpstk;
%}

%include "../../../src/BivarStats.hpp"