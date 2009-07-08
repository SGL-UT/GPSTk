//swig interface for BinexData.hpp

%module gpstk_BinexData

%{
#include "../../../src/BinexData.hpp"

using namespace std;
using namespace gpstk;
%}

%include "../../../src/BinexData.hpp"
