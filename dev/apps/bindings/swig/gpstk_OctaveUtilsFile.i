//swig file to fix streaming

%module gpstk_OctaveUtilsFile

%{
#include "gpstk_OctaveUtils.cpp"


typedef RinexObsHeader::RinexObsType RinexObsType;
typedef RinexObsHeader::ExtraWaveFact ExtraWaveFact;
typedef RinexObsData::RinexDatum RinexDatum;
typedef RinexObsData::RinexPrnMap RinexPrnMap;
using namespace gpstk;
%}

%include "gpstk_OctaveUtils.i"
