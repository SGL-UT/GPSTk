//swig interface for FFStream.hpp

%module gpstk_FFStream

%{
#include <fstream>
#include "../../../src/FFStream.hpp"

using namespace std;
using namespace gpstk;
%}
%import "../../../src/FFStreamError.hpp"

%include "../../../src/FFStream.hpp"
%include "gpstk_FFTextStream.i"
%include "gpstk_Matrix.i"
%include "gpstk_FFBinaryStream.i"
%include "gpstk_BinexStream.i"
%include "gpstk_BLQDataReader.i"
%include "gpstk_ConfDataReader.i"
%include "gpstk_FICStreamBase.i"
%include "gpstk_FICAStream.i"
%include "gpstk_FICStream.i"
%include "gpstk_IonexStream.i"
%include "gpstk_MSCStream.i"
%include "gpstk_RinexMetStream.i"
%include "gpstk_RinexNavStream.i"
%include "gpstk_RinexObsStream.i"
%include "gpstk_SatDataReader.i"
%include "gpstk_SEMStream.i"
%include "gpstk_SP3Stream.i"
%include "gpstk_YumaStream.i"



