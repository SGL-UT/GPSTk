//swig interface for FFBinaryStream

%module gpstk_FFBinaryStream

%{
#include "../../../src/FFStream.hpp"
#include "../../../src/FFBinaryStream.hpp"
#include "../../../src/BinexStream.hpp"

using namespace std;
using namespace gpstk;
%}

%include "../../../src/FFStream.hpp"
%include "../../../src/FFBinaryStream.hpp"
%include "../../../src/BinexStream.hpp"
