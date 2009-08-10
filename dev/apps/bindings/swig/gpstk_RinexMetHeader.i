//swig interface for RinexMetHeader.hpp

%module RinexMetHeader

%{
#include "../../../src/RinexMetHeader.hpp"

using namespace gpstk;
%}

%include "../../../src/RinexMetHeader.hpp"

%{
typedef RinexMetHeader::RinexMetType RinexMetType;
typedef RinexMetHeader::sensorType sensorType;
typedef RinexMetHeader::sensorPosType sensorPosType;
%}

