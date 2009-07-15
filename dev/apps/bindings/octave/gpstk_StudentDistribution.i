//swig interface for StudentDistribution.hpp

%module gpstk_StudentDistribution

%{
#include "../../../src/BaseDistribution.hpp"
#include "../../../src/StudentDistribution.hpp"

using namespace gpstk;
%}

%include "../../../src/BaseDistribution.hpp"
%include "../../../src/StudentDistribution.hpp"
