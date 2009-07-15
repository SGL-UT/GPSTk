//swig interfave for FileStore.hpp

%module gpstk_FileStore

%{
#include "../../../src/FileStore.hpp"

using namespace gpstk;
%}

%include "../../../src/FileStore.hpp"