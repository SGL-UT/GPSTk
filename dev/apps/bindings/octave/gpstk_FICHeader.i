//swig interface for FICHeader.hpp

%module gpstk_FICHeader

%{
#include "../../../src/FICHeader.hpp"

using namespace gpstk;
%}

%include "../../../src/FICHeader.hpp"

%{
typedef FICHeader::reallyPutRecord(FFStream& s) reallyPutRecord(FFStream& s);
typedef FICHeader::reallyGetRecord(FFStream& s) reallyGetRecord(FFStream& s);
%}
