//swig interface for BivarStats.hpp

%module gpstk_BivarStats

%{
#include "../../../src/BivarStats.hpp"

using namespace std;
using namespace gpstk;
%}

%rename (BivarStats_plusequal) operator+=;
%include "../../../src/BivarStats.hpp"
%rename (plusequal) operator+=;
