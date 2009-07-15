//swig interface for SEMAlmanacStore.hpp

%module gpstk_SEMAlmanacStore

%{
#include "../../../src/GPSAlmanacStore.hpp"
#include "../../../src/FileStore.hpp"
#include "../../../src/XvtStore.hpp"
#include "../../../src/SEMHeader.hpp"
#include "../../../src/SEMAlmanacStore.hpp"

using namespace gpskt;
%}

%include "../../../src/GPSAlmanacStore.hpp"
%include "../../../src/FileStore.hpp"
%include "../../../src/XvtStore.hpp"
%include "../../../src/SEMHeader.hpp"
%include "../../../src/SEMAlmanacStore.hpp"