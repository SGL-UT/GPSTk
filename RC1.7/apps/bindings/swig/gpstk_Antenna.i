//swig interface for Antenna.hpp

%module gpstk_Antenna

%{
#include "../../../src/Antenna.hpp"

using namespace std;
using namespace gpstk;
%}

%include "../../../src/Antenna.hpp"