//swig interface for SP3SatID.hpp

%module gpstk_SP3SatID

%{
#include "../../../src/SatID.hpp"
#include "../../../src/SP3SatID.hpp"

using namespace gpstk;
%}

%include "../../../src/SatID.hpp"
%include "../../../src/SP3SatID.hpp"