//swig interface for Position.hpp

%module gpstk_Position

%{
#include "../../../src/Position.hpp"

using namespace gpstk;
%}

%rename (Position_streamRead) operator<<;
%rename (Position_minus) operator-;
%rename (Position_plus) operator+;
%rename (Position_starMult) operator*;
%include "../../../src/Position.hpp"
%rename (streamRead) operator<<;
%rename (minus) operator-;
%rename (plus) operator+;
%rename (starMult) operator*;

