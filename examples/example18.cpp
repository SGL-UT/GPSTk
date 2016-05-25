//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file example18.cpp
 * 
 */
 
// Example program Nro 18 for GPSTk
//
// This program shows how to use 'GeneralEquations' and 'GeneralConstraint' 
// together with 'SolverGeneral' solve complex problems with Kalman filter. 
//
// To show the outline of the processing framework clearly, this example won't 
// process some real GNSS data, but try to solver the following mathematical 
// problem(the true value of x1 x2 and x3 is 1.0 2.0 and 4.0):
//
//    y1 = x1 + x2 +x3                               equ.(1)
//    y2 = x2+x3                                     equ.(2)
//
// It's clear that the above equations is rand defect, and an additional 
// constraint is added to solver the problem:
//
//    x3 = 4.0                                       equ.(3)
//
// Now, x1 x2 and x3 can be solved. For gnss data processing, we usually feed back
// the fixed integer ambiguity to the solver to improve the solution, the following
// show how to feed back some other constraint to the solver.
//
//    x1 = 1.0                                       equ.(4)
//
// Rank defect is the key problem for some PPP-RTK algorithms, and this framework
// is designed to implement these algorithms gracefully. And more examples will 
// added to show how to use these classes to do RTK and PPP-RTK in the near future.
//
//



#include <iostream>
#include "SolverGeneral.hpp"
#include "GeneralConstraint.hpp"
#include "BasicFramework.hpp"
#include "TypeID.hpp"
#include "random.hpp"

using namespace std;
using namespace gpstk;

class MyEquations : public GeneralEquations
{
public:
   MyEquations(){}
   virtual ~MyEquations(){}

protected:
   virtual EquationSystem getEquations()
   {
      // y1 = x1 + x2 +x3
      // y2 = x2 + x3

      Variable x1( TypeID::byName("x1"), &defaultModel,
                   true, false, 9e10, 1.0, true );
      Variable x2( TypeID::byName("x2"), &defaultModel,
                   true, false, 9e10, 1.0, true );
      Variable x3( TypeID::byName("x3"), &defaultModel,
                   true, false, 9e10, 1.0, true );

      Variable y1(TypeID::byName("y1"));
      Variable y2(TypeID::byName("y2"));
      
      // y1 = x1 + x2 + x3
      Equation equ1(y1);
      equ1.addVariable(x1);
      equ1.addVariable(x2);
      equ1.addVariable(x3);

      // y2 = x2 + x3
      Equation equ2(y2);
      equ2.addVariable(x2);
      equ2.addVariable(x3);
      
      EquationSystem sys;
      sys.addEquation(equ1);
      sys.addEquation(equ2);

      return sys;
   }

   virtual ConstraintSystem getConstraints(gnssRinex& gRin)
   {
      // x3 = 4.0

      ConstraintSystem constraintSystem;
      
      Variable var;
      var.setType(TypeID::byName("x3"));

      Constraint cons;
      cons.header = constraintHeader(4.0,1e-12);
      cons.body[var] = 1.0;
      
      constraintSystem.addConstraint(cons);

      return constraintSystem;
   }

private:
   StochasticModel defaultModel;
};

class MyConstraint : public GeneralConstraint
{
public:
   MyConstraint(SolverGeneral& solverGeneral)
      : GeneralConstraint(solverGeneral){}      
   virtual ~MyConstraint(){}

protected:

   virtual void realConstraint(gnssDataMap& gdsMap)
   {
      // x1 = 1.0

      VariableSet varOfX1 = getVariables(SourceID(),TypeID::byName("x1"));
      VariableSet varOfX2 = getVariables(SourceID(),TypeID::byName("x2"));
      VariableSet varOfX3 = getVariables(SourceID(),TypeID::byName("x3"));
      
      ConstraintSystem sys;
      
      Constraint constraint;
      constraint.header = constraintHeader(1.0,1e-12);
      constraint.body[*varOfX1.begin()] = 1.0;

      sys.addConstraint(constraint);

      constraintToSolver(sys,gdsMap);

   }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
   virtual void updateRefSat( const SatSourceMap& refsatSource,
                               const SourceSatMap& sourceRefsat )
   {
      // No need to do anything for our case
   }

};
#pragma clang diagnostic pop
const string APP_NAME = "MyApp";
const string APP_VERSION = "1.0.0";
const string APP_DESC = 
"Simple example to show how to use constraint system classes\n" \
"...";

class MyApp : public BasicFramework
{
public:
   MyApp(): BasicFramework(APP_NAME + " " + APP_VERSION, APP_DESC)
   {
         // We register some new TypeIDs for the equation system
      TypeID::regByName("x1","x1...");
      TypeID::regByName("x2","x2...");
      TypeID::regByName("x3","x3...");
      TypeID::regByName("x4","x4...");
      TypeID::regByName("y1","y1...");
      TypeID::regByName("y2","y2...");
      TypeID::regByName("y3","y3...");
   }

   virtual ~MyApp() 
   {
         // We clear the registered TypeIDs
      TypeID::unregAll();
   }

protected:

   virtual void spinUp()
   {
      cout << "This is example 18 of GPSTk. " << endl;
   }

   virtual void process()
   {
      cout << "process..." << endl;

      cout << fixed;
      cout << setprecision(6);
      
         // Simulate some data
      const int obsNum(100);

      const double xx0[3]={1.0,2.0,4.0};

      Vector<double> x0(3,0.0);
      x0 = xx0;

      Matrix<double> obsX(obsNum,3,0.0);
      Matrix<double> obsY(obsNum,2,0.0);

      for(int i=0;i<obsNum;i++)
      {
         obsX[i][0] = x0[0] + RandNorm(0.005);
         obsX[i][1] = x0[1] + RandNorm(0.005);
         obsX[i][2] = x0[2] + RandNorm(0.005);

         obsY[i][0] =  obsX[i][0]+obsX[i][1]+obsX[i][2] ;
         obsY[i][1] =  obsX[i][1]+obsX[i][2] ;
      }


         // process the data
      MyEquations equations;
      SolverGeneral solver(equations.getEquationSystem());
      MyConstraint constraint(solver);
      
      for(int i=0;i<obsNum;i++)
      {
         gnssRinex gRin;
         SatID sat;
         
         gRin.body[sat][TypeID::byName("x1")] = obsX[i][0];
         gRin.body[sat][TypeID::byName("x2")] = obsX[i][1];
         gRin.body[sat][TypeID::byName("x3")] = obsX[i][2];

         gRin.body[sat][TypeID::byName("y1")] = obsY[i][0];
         gRin.body[sat][TypeID::byName("y2")] = obsY[i][1];

         gRin.body[sat][TypeID::elevation] = 0.0;
         gRin.body[sat][TypeID::CSL1] = 0.0;

         constraint.process(gRin, &equations);

         double x1 = solver.getSolution(TypeID::byName("x1"));
         double x2 = solver.getSolution(TypeID::byName("x2"));
         double x3 = solver.getSolution(TypeID::byName("x3"));

         cout << setw(4) << i << " "
              << setw(10) << x1 << " "
              << setw(10) << x2 << " "
              << setw(10) << x3 << endl;

      }

   }

   virtual void shutDown()
   {
      cout << "Finished." << endl;
   }

private:

   gnssDataMap gdsMap;

};


// Entry point of the program

GPSTK_START_MAIN(MyApp)