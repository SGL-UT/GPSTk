//swig interface for FICStream.hpp

%module gpstk_FICStream

%{
#include "../../../src/FICStreamBase.hpp"
#include "../../../src/FFBinaryStream.hpp"
#include "../../../src/FICStream.hpp"

using namespace gpstk;
%}

%include "../../../src/FICStreamBase.hpp"
%include "../../../src/FFBinaryStream.hpp"
%include "../../../src/FICStream.hpp"
