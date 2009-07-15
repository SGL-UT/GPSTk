//swig interface for Position.hpp

%module gpstk_Position

%{
#include "../../../src/Position.hpp"

using namespace gpstk;
%}

%include "../../../src/Position.hpp"