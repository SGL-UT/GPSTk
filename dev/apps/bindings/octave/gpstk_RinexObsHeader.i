//swig interface for RinexObsHeader.hpp

%module gpstk_RinexObsHeader

%{

#include "../../../src/RinexObsHeader.hpp"

using namespace gpstk;
%}


%include "../../../src/RinexObsHeader.hpp"

%{
typedef RinexObsHeader::RinexObsType RinexObsType;
typedef RinexObsHeader::ExtraWaveFact ExtraWaveFact;
%}
