//swig interface for FFData.hpp

%module gpstk_FFData

%{
#include "../../../src/FFData.hpp"

using namespace std;
using namespace gpstk;
%}

%rename (BinexData_opequal) operator=;
%rename (FFData_streamRead) operator<<;
%rename (streamWrite) operator>>;
%rename (RinexNavData_EngEphemeris) operator EngEphemeris;
%rename (SEMData_AlmOrbit) operator AlmOrbit;
%include "../../../src/FFData.hpp"
%include "gpstk_BinexData.i"
%include "gpstk_FICBase.i"
%include "gpstk_FICHeader.i"
%include "gpstk_FICFilterOperators.i"
%include "gpstk_MSCBase.i"
%include "gpstk_MSCData.i"
%include "gpstk_MSCHeader.i"
%include "gpstk_RinexObsBase.i"
%include "gpstk_RinexObsData.i"
%include "gpstk_RinexNavBase.i"
%include "gpstk_RinexNavData.i"
%include "gpstk_RinexObsHeader.i"
%include "gpstk_RinexMetBase.i"
%include "gpstk_RinexMetHeader.i"
%include "gpstk_RinexNavHeader.i"
%include "gpstk_SEMBase.i"
%include "gpstk_SEMData.i"
%include "gpstk_SEMHeader.i"
%include "gpstk_SP3Base.i"
%include "gpstk_SP3Data.i"
%include "gpstk_SP3Header.i"
%include "gpstk_YumaBase.i"
%include "gpstk_YumaData.i"
%include "gpstk_YumaHeader.i"
%include "gpstk_FICDataSet.i"
%include "gpstk_IonexBase.i"
%include "gpstk_IonexData.i"
%include "gpstk_IonexHeader.i"
%include "gpstk_RinexMetData.i"
%include "gpstk_RinexNavData.i"
%rename (opequal) operator=;
%rename (streamRead) operator<<;
%rename (EngEphemeris) operator EngEphemeris;
%rename (AlmOrbit) operator AlmOrbit;

