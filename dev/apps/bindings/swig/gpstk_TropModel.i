//swig interface for TropModel.hpp

%module gpstk_TropModel

%{
#include "../../../src/TropModel.hpp"

using namespace gpstk;
%}

%include "../../../src/TropModel.hpp"

%{
typedef TropModel::InvalidTropModel InvalidTropModel;
%}
