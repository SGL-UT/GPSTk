//swig interface for ObsID.hpp

%module gpstk_ObsID

%{
#include "../../../src/ObsID.hpp"

using namespace gpstk;
using namespace StringUtils;
%}

%ignore  _ZN5gpstk5ObsID9singletonE;
%include "../../../src/ObsID.hpp"
%include "gpstk_RinexObsID.i"

%{
typedef ObsID::Initializer Initializer;
%}

