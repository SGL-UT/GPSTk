//swig interface for RungeKutta4.hpp

%module gpstk_RungeKutta4

%{
#include "../../../src/RungeKutta4.hpp"

using namespace gpstk;
%}

%include "../../../src/RungeKutta4.hpp"