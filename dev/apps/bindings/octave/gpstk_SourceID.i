//swig interface for SourceID.hpp

%module gpstk_SourceID

%{
#include "../../../src/SourceID.hpp"

using namespace gpstk;
using namespace StringUtils;
%}

%ignore SourceIDsingleton;
%rename (SourceID_opequal) operator=;
%include "../../../src/SourceID.hpp"

%{
typedef SourceID::Initializer Initializer;
%}
