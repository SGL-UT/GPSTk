//swig interface for FFStream.hpp

%module gpstk_FFStream

%{
#include "../../../src/FFStream.hpp"
#include "../../../src/FFTextStream.hpp"
#include "../../../src/FFBinaryStream.hpp"
#include "../../../src/BinexStream.hpp"
//#include "../../../src/BLQDataReader.hpp"
#include "../../../src/ConfDataReader.hpp"
#include "../../../src/FICStreamBase.hpp"
#include "../../../src/FICAStream.hpp"
#include "../../../src/FICStream.hpp"
#include "../../../src/IonexStream.hpp"
#include "../../../src/MSCStream.hpp"
#include "../../../src/RinexMetStream.hpp"
#include "../../../src/RinexNavStream.hpp"
#include "../../../src/RinexObsStream.hpp"
#include "../../../src/SatDataReader.hpp"
#include "../../../src/SEMStream.hpp"
#include "../../../src/SP3Stream.hpp"
#include "../../../src/YumaStream.hpp"

using namespace std;
using namespace gpstk;
%}

%include "../../../src/FFStream.hpp"
%include "../../../src/FFTextStream.hpp"
%include "../../../src/FFBinaryStream.hpp"
%include "../../../src/BinexStream.hpp"
//%include "../../../src/BLQDataReader.hpp"
%include "../../../src/ConfDataReader.hpp"
%include "../../../src/FICStreamBase.hpp"
%include "../../../src/FICAStream.hpp"
%include "../../../src/FICStream.hpp"
%include "../../../src/IonexStream.hpp"
%include "../../../src/MSCStream.hpp"
%include "../../../src/RinexMetStream.hpp"
%include "../../../src/RinexNavStream.hpp"
%include "../../../src/RinexObsStream.hpp"
%include "../../../src/SatDataReader.hpp"
%include "../../../src/SEMStream.hpp"
%include "../../../src/SP3Stream.hpp"
%include "../../../src/YumaStream.hpp"



