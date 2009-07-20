//swig interface for FFData.hpp

%module gpstk_FFData

%{
#include "../../../src/FFData.hpp"
#include "../../../src/BinexData.hpp"
#include "../../../src/FICBase.hpp"
#include "../../../src/RinexObsBase.hpp"
#include "../../../src/RinexObsData.hpp"

using namespace gpstk;
%}

%rename (FFData_streamRead) operator<<;
%rename (FFData_streamRead) operator>>;
%include "../../../src/FFData.hpp"
%include "../../../src/BinexData.hpp"
%include "../../../src/FICBase.hpp"
%include "../../../src/RinexObsBase.hpp"
%include "../../../src/RinexObsData.hpp"
%include "gpstk_FICDataSet.i"
%include "gpstk_IonexData.i"
%include "gpstk_IonexHeader.i"
%include "gpstk_MSCHeader.i"
%include "gpstk_RinexMetData.i"
%include "gpstk_RinexNavData.i"
%include "gpstk_YumaData.i"
%rename (streamRead) operator<<;
%rename (streamRead) operator>>;


