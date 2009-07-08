//swig interface for BinexFilterOperators.hpp

%module gpstk_BinexFilterOperators

%{
#include "../../../src/BinexFilterOperators.hpp

using namespace std;
using namespace gpstk;
%}

typedef std::binary_function<BinexData, BinexData, bool> BinexDataBinaryOperator;

%include "../../../src/BinexFilterOperators.hpp"

struct BinexDataOperatorEquals : public BinexDataBinaryOperator
