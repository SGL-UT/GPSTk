//swig interface fo CodeBuffer.hpp

%module gpstk_CodeBuffer

%{
#include "../../../src/CodeBuffer.hpp"

using namespace gpstk;
%}

%rename (CodeBuffer_opequal) operator=;
%rename (CodeBuffer_carrotequal) operator^=;
%rename (Toolkit_CodeBuffer_operator)  gpstk::CodeBuffer::operator [](int const) const;
%include "../../../src/CodeBuffer.hpp"
