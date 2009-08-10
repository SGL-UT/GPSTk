//swig interface for GaussianDistribution.hpp

%module gpstk_GaussianDistribution

%{
#include "../../../src/GaussianDistribution.hpp"

using namespace gpstk;
%}

%include "../../../src/GaussianDistribution.hpp"
