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

%{
typedef Epoch::EpochException EpochException;
typedef Epoch::FormatException FormatException;
%}
