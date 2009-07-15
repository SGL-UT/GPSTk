//swig interface for IonoModel.hpp

%module gpstk_IonoModel

%{
#include "../../../src/IonoModel.hpp"

using namespace gpstk;
%}

%include "../../../src/IonoModel.hpp"

%{
//typedef gpstk::IonoModel::InvalidIonoModel;  gpstk::IonoModel;
typedef IonoModel::InvalidIonoModel InvalidIonoModel;
%}
