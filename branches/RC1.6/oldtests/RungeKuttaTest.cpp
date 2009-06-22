#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

// C++ includes
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>

#include "RungeKutta4.hpp"

/**
 * @file RungeKuttaTest.cpp
 *
 */

using namespace gpstk;
using namespace std;

// The full, nonlinear equation of motion for a simple pendulum
class PendulumIntegrator : public RungeKutta4
{
   public:

      PendulumIntegrator(const Matrix<double>& initialState,
                         double initialTime=0)
            : RungeKutta4(initialState, initialTime)
      {};     

      virtual gpstk::Matrix<double>&
         derivative(long double time, 
                    const gpstk::Matrix<double>& inState,
                    gpstk::Matrix<double>& inStateDot);

      void setPhysics(double accGrav, double length) 
      {  g = accGrav; L = length; };
   
   private: 

   double g; //< the acceleration due to gravity
   double L; //< the length of the pendulum (in meters?)
};

gpstk::Matrix<double>&
PendulumIntegrator::derivative(long double time, 
                               const gpstk::Matrix<double>& inState,
                               gpstk::Matrix<double>& inStateDot)
{
   inStateDot(0,0) = inState(1,0); // velocity along x 
   inStateDot(1,0) = -g/L * sin(inState(0,0));
   return inStateDot;
}

int  main ()
{
   gpstk::Matrix<double> x0(2,1), truncError(2,1);
   x0(0,0)  = 0.001; // Initial angle in radians
   x0(1,0)  = 0.0;  // Initial angular velocity in radians/second
   
   PendulumIntegrator pModel(x0,0.);

   double g=9.81, L=1.0;
   pModel.setPhysics(g,L); 

      // Formatted for easy reading into Octave (www.octave.org) 
   cout << "# Pendulum motion result" << endl;
   cout << "# Columns: Time, Theta, d Theta/ dt, ";
   cout << "estimated error in theta, theta dot" << endl;
   
   double deltaT = .01;  // Step size in seconds for integrator

   double time = 0;
   double Nper = 2; // number of periods

      // Output state over approximately Nper periods (exactly for
      // small oscillations)

   long count = 0;
   while (pModel.getTime() < Nper * (2 * 3.14159265/sqrt(g/L)))
   {  
      pModel.integrateTo((count++)*deltaT,truncError);

      cout << setprecision(12)
           << pModel.getTime() << " " 
           << pModel.getState()(0,0) << " " << pModel.getState()(1,0) << " " 
           << truncError(0,0) << " " << truncError(1,0) << endl;
   }

  return(0);
}
