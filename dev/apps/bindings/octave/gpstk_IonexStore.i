//swig interface for IonexStore.hpp

%module gpstk_IonexStore

%{
#include "../../../src/FileStore.hpp"
#include "../../../src/IonexStore.hpp"

using namespace gpstk;
%}

%include "../../../src/FileStore.hpp"
%include "../../../src/IonexStore.hpp"