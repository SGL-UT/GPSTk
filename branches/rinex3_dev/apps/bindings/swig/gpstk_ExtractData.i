//swig interface for ExtractData.hpp

%module gpstk_ExtractData

%{
#include "../../../src/ExtractData.hpp"


using namespace gpstk;
%}

%include "../../../src/ExtractData.hpp"
%include "gpstk_ExtractC1.i"
%include "gpstk_ExtractCombinationData.i"
%include "gpstk_ExtractD1.i"
%include "gpstk_ExtractD2.i"
%include "gpstk_ExtractL1.i"
%include "gpstk_ExtractL2.i"
%include "gpstk_ExtractLC.i"
%include "gpstk_ExtractP1.i"
%include "gpstk_ExtractP2.i"
%include "gpstk_ExtractPC.i"
