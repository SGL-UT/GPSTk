//swig interface for GaussianDistribution.hpp

%module gpstk_GaussianDistribution

%{
#include "../../../src/BaseDistribution.hpp"
#include "../../../src/GaussianDistribution.hpp"

using namespace gpstk;
%}

%include "../../../src/BaseDistribution.hpp"
%include "../../../src/GaussianDistribution.hpp"