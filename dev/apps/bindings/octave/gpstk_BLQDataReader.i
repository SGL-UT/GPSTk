//swig interface for BLQDataReader

%module gpstk_BLQDataReader

%{
//#include "../../../src/FFTextStream.hpp"
//#include "../../../src/FFStream.hpp"
#include "../../../src/BLQDataReader.hpp"

using namespace std;
using namespace gpstk;
%}

//%include "../../../src/FFTextStream.hpp"
//%include "../../../src/FFStream.hpp"
%include "../../../src/BLQDataReader.hpp"
