//swig interface for GPSZcount

%module gpstk_GPSZcount

%{
#include "../../../src/GPSZcount.hpp"

using namespace gpstk;
%}

%rename (GPSZcount_plusequal) operator+=;
%rename (GPSZcount_minusequal) operator-=;
%include "../../../src/GPSZcount.hpp"
