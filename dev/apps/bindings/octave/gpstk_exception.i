// GPSTk wrap for exception.cpp


%module gpstk_exception
%{

#include "../../../src/Exception.hpp"

using namespace std;
using namespace gpstk;
%}

%include stl.i
//%include "gpstk_Triple.i"
//%include "gpstk_Zcount.i"

// Exception
// Redefinition problem, similar to Triple & Xvt, also at operator<<
// Exception+DayTimeException and Triple+Xvt pairs don't
//   redefine each other's operator<<'s
// Fixing method, see Xvt
%rename (Exception_opequal) operator=;
%rename (Exception_streamRead) operator<<;
%include "../../../src/Exception.hpp"
//%include "gpstk_StringUtils.i"
%rename (opequal) operator=;
%rename (streamRead) operator<<;

