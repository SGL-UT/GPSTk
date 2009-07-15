//swig interface for MJD.hpp"

%module gpstk_MJD

%{
#include "../../../src/TimeTag.hpp"
#include "../../../src/MJD.hpp"

using namespace gpstk;
%}

%include "../../../src/TimeTag.hpp"
%include "../../../src/MJD.hpp"
