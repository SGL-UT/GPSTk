//swig interface for MSCStore.hpp

%module gpstk_MSCStore

%{
#include "../../../src/XvtStore.hpp"
#include "../../../src/FileStore.hpp"
#include "../../../src/MSCStore.hpp"

using namespace gpstk;
%}

%include "../../../src/XvtStore.hpp"
%include "../../../src/FileStore.hpp"
%include "../../../src/MSCStore.hpp"