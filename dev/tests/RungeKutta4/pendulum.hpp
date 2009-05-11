#pragma ident "$Id$"
//file: pendulum.hpp

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
//============================================================================

#include <math.h>
#include "RungeKutta4.hpp"

using namespace gpstk;
using namespace std;

//A pendulum is of the equation d^2x/dt^2 + (g/L)*sin(x) = 0

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
