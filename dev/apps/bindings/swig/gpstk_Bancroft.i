//swig interface for Bancroft.hpp

%module gpstk_Bancroft

%{
#include "../../../src/Bancroft.hpp"

using namespace std;
using namespace gpstk;
%}

%rename (ToolKit_Bancroft_Compute) gpstk::Bancroft::Compute(Matrix< double > const &,Vector< double > &);
%include "../../../src/Bancroft.hpp"
