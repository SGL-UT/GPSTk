//swig interfave for FileStore.hpp

%module gpstk_FileStore

%{
#include "../../../src/FileStore.hpp"

using namespace gpstk;
%}

%include "../../../src/FileStore.hpp"
%include "gpstk_IonexStore.i"
%include "gpstk_MSCStore.i"
%include "gpstk_RinexEphemerisStore.i"
%include "gpstk_SP3EphemerisStore.i"
%include "gpstk_YumaAlmanacStore.i"
