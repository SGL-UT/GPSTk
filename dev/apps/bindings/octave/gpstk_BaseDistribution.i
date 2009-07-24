//swig interface for BaseDistribution.hpp

%module gpstk_BaseDistribution

%{
#include "../../../src/BaseDistribution.hpp"

using namespace std;
using namespace gpstk;
%}

%include "../../../src/BaseDistribution.hpp"
%include "gpstk_Chi2Distribution.i"
