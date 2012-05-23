//swig interface for BinexStream.hpp

%module gpstk_BinexStream

%{
#include "../../../src/BinexStream.hpp"

using namespace std;
using namespace gpstk;
%}

%include "../../../src/BinexStream.hpp"
