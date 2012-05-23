//swig interface for SimpleKalmanFilter.hpp

%module gpstk_SimpleKalmanFilter

%{
#include "../../../src/SimpleKalmanFilter.hpp"

using namespace gpstk;
%}

%include "../../../src/SimpleKalmanFilter.hpp"