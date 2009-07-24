//swig interface for Matrix.hpp

%module gpstk_Matrix
%{
#include "../../../src/Matrix.hpp"

using namespace gpstk;
%}

%rename (Matrix_opequal) operator=;
%include "../../../src/Matrix.hpp"
%include "gpstk_MatrixBase.i"
%include "gpstk_MatrixBaseOperators.i"
%include "gpstk_MatrixImplementation.i"
%include "gpstk_MatrixFunctors.i"
%include "gpstk_DOP.i"
%include "gpstk_PRSolution.i"
%include "gpstk_Bancroft.i"
%include "gpstk_ModeledPseudorangeBase.i"
%include "gpstk_RungeKutta4.i"
%include "gpstk_SolverBase.i"
%include "gpstk_SimpleKalmanFilter.i"



