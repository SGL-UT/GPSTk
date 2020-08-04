%{
#include "ValidType.hpp"
#include "Matrix.hpp"
%}

%include "ValidType.hpp"
%include "Matrix.hpp"

// ValidType wraps
%template(validtype_float) gpstk::ValidType<float>;
%template(validtype_double) gpstk::ValidType<double>;
%template(validtype_char) gpstk::ValidType<char>;
%template(validtype_short) gpstk::ValidType<short>;
%template(validtype_int) gpstk::ValidType<int>;
%template(validtype_long) gpstk::ValidType<long>;
%template(validtype_uchar) gpstk::ValidType<unsigned char>;
%template(validtype_ushort) gpstk::ValidType<unsigned short>;
%template(validtype_uint) gpstk::ValidType<unsigned int>;
%template(validtype_ulong) gpstk::ValidType<unsigned long>;

// Matrix wraps
%template(matrix_double) gpstk::Matrix<double>;
