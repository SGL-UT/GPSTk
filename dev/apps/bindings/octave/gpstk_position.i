// swig file for Triple.cpp

%module gpstk_position

%{

#include "../../../src/Triple.hpp"							
#include "../../../src/Position.hpp"
#include "../../../src/ECEF.hpp"
#include "../../../src/Xvt.hpp"
#include "../../../src/Geodetic.hpp"
//#include "../../../src/RAIMSolution.hpp"


using namespace std;
using namespace gpstk;

%}

// Position Group
//
// Triple
// Rename below to solve "shadowed by..." warning
%rename (operator_bracket_const) gpstk::Triple::operator[](size_t const) const;	
// Under Triple.hpp, friend std::ostream operator<< will
//    clash with Xvt's def of operator<<, result in redefine
%rename (Position_starMult) operator*;
%rename (Position_starMult) operator*;
%rename (Triple_streamRead) operator<<;						
%rename (Position_minus) operator-;
%rename (Position_plus) operator+;
%include "../../../src/Triple.hpp"							
%rename (streamRead) operator<<;							
//Position
%rename (printf_const) printf(std::string const &) const;
%include "../../../src/Position.hpp"
// ECEF
%include "../../../src/ECEF.hpp"
// GeoidModel
%include "../../../src/GeoidModel.hpp"
// Xvt
// The %rename directive below solves the redefinition conflict with Triple.hpp
%rename (starMult) operator*;
%rename (minus) operator-;
%rename (plus) operator+;
%rename (Xvt_streamRead) operator<<;
%include "../../../src/Xvt.hpp"
// The %rename directive below restores the usual naming scheme
%rename (streamRead) operator<<;
// Geodetic
%include "../../../src/Geodetic.hpp"
// RAIM Solution
//%include "../../../src/RAIMSolution.hpp"

