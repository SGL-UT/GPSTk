//swig interface for RinexMetHeader.hpp

%module RinexMetHeader

%{
#include "../../../src/RinexMetBase.hpp"
#include "../../../src/RinexMetHeader.hpp"

using namespace gpstk;
%}

%include "../../../src/RinexMetBase.hpp"
%include "../../../src/RinexMetHeader.hpp"

typedef RinexMetHeader :: SensorType SensorType;
typedef RinexMetHeader :: SensorPosType SensorPosType;
