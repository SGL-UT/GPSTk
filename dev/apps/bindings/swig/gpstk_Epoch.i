//swig interface for Epoch.hpp

%module gpstk_Epoch

%{
#include "../../../src/Epoch.hpp"

using namespace gpstk;
%}

%rename (Epoch_CommonTime) operator CommonTime;
%rename (Epoch_GPSZcount) operator GPSZcount;
%rename (Epoch_opequal) operator=;
%rename (Epoch_plusequal) operator+=;
%rename (Epoch_minequal)operator-=;
%ignore END_OF_TIME;
%ignore BEGINNING_OF_TIME;
%include "../../../src/Epoch.hpp"
%rename (CommonTime) operator CommonTime;
%rename (GPSZcount) operator GPSZcount;
%rename (opequal) operator=;
%rename (plusequal) operator+=;
%rename (minequal)operator-=;

%{
typedef Epoch::EpochException EpochException;
typedef Epoch::FormatException FormatException;
%}
