//swig interface for SatDataReader.hpp

%module gpstk_SatDataReader

%{
#include "../../../src/SatDataReader.hpp"

using namespace std;
using namespace gpstk;
%}

%include "../../../src/SatDataReader.hpp"
