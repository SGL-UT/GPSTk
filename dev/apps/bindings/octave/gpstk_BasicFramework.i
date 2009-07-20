//swig interface for BasicFramework.hpp

%module gpstk_BasicFramework

%{
#include "../../../src/BasicFramework.hpp"
#include "../../../src/InOutFramework.hpp"

using namespace std;
using namespace gpstk;
%}

%include "../../../src/BasicFramework.hpp"
%include "../../../src/InOutFramework.hpp"
