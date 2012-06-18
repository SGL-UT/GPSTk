//swig interface for IonoModelStore.hpp

%module gpstk_IonoModelStore

%{
#include "../../../src/IonoModelStore.hpp"

using namespace gpstk;
%}

%include "../../../src/IonoModelStore.hpp"

%{
typedef IonoModelStore::NoIonoModelFound NoIonoModelFound;
%}
