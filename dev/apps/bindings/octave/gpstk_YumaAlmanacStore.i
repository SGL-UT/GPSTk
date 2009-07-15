//swig interface for YumaAlmanacStore.hpp

%module gpstk_YumaAlmanacStore

%{
#include "../../../src/GPSAlmanacStore.hpp"
#include "../../../src/FileStore.hpp"
#include "../../../src/YumaAlmanacStore.hpp"

using namespace gpstk;
%}

%include "../../../src/GPSAlmanacStore.hpp"
%include "../../../src/FileStore.hpp"
%include "../../../src/YumaAlmanacStore.hpp"