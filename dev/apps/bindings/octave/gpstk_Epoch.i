//swig interface for Epoch.hpp

%module gpstk_Epoch

%{
#include "../../../src/Epoch.hpp"

using namespace gpstk;
%}

%rename (Epoch_CommonTime) operator CommonTime;
%rename (Epoch_GPSZcount) operator GPSZcount;
%include "../../../src/Epoch.hpp"
%rename (CommonTime) operator CommonTime;
%rename (GPSZcount) operator GPSZcount;

%{
typedef Epoch::EpochException EpochException;
typedef Epoch::FormatException FormatException;
%}
