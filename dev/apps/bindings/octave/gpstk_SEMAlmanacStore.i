//swig interface for SEMAlmanacStore.hpp

%module gpstk_SEMAlmanacStore

%{
#include "../../../src/SEMAlmanacStore.hpp"

using namespace gpstk;
%}

%template() FileStore<SEMHeader>;
%include "../../../src/SEMAlmanacStore.hpp"


