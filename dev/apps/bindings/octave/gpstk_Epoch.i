//swig interface for Epoch.hpp

%module gpstk_Epoch

%{
#include "../../../src/Epoch.hpp"

using namespace gpstk;
%}

%include "../../../src/Epoch.hpp"

%{
typedef Epoch::EpochException EpochException;
typedef Epoch::FormatException FormatException;
%}
