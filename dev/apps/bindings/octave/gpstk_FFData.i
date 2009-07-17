//swig interface for FFData.hpp

%module gpstk_FFData

%{
#include "../../../src/FFData.hpp"
#include "../../../src/RinexObsBase.hpp"
#include "../../../src/RinexObsData.hpp"

using namespace gpstk;
%}

%rename (FFData_streamRead) operator<<;
%rename (FFData_streamRead) operator>>;
%include "../../../src/FFData.hpp"
%include "../../../src/RinexObsBase.hpp"
%include "../../../src/RinexObsData.hpp"
%rename (streamRead) operator<<;
%rename (streamRead) operator>>;


