#pragma ident "$Id: RungeKutta4.hpp 70 2006-08-01 18:36:21Z ehagen $"



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


/*
 * @file RKIntegrator.hpp
 * RKIntegrator class. Provides a common interface to the Runge Kutta
 * family of numerical integrators.
 */

#ifndef GPSTK_RKINTEGRATORS_H
#define GPSTK_RKINTEGRATORS_H

#include "Matrix.hpp"

namespace gpstk
{

   /** @addtogroup math */
   //@{

      /** A virtual base class for the Rune Kutta family of integrators.
       *
       */
   class RKIntegrator 
   {
   public:
         /** Constructor.
          * @param initalState a reference to the original Matrix to work on 
          *  that is copied to an internal Matrix.
          * @param initialTime the time at which to begin integrations 
          * @param timeEpsilon how close the final internal timestep must match
          *  the specified final time of an integration
          */
      RKIntegrator(const Matrix<double>& initialState,
                  double initialTime=0,
                  double timeEpsilon=1e-18)
            : currentState(initialState), currentTime(initialTime), 
              teps(timeEpsilon), M(initialState.rows()), N(initialState.cols())
         { }     

         /** 
          * This is the single step integration routine. Subclasses must define this function.
          * @param nextTime the time to integrate to
          * @param stepSize the amount time between internal integration steps
          */
      virtual void integrateTo (double nextTime, 
                        double stepSize = 0) = 0;

         /** A single step integration that also provides an estimate of error. Subclasses must define this function.
          * @param nextTime the time to integrate to
          * @param error the Matrix of estimated integration error 
          *  (one for each element)
          * @param stepSize the amount time between internal integration steps
          */
      virtual void integrateTo (double nextTime,
                        Matrix<double>& error,
                        double stepSize = 0) = 0;

         /** This is the function to be integrated. Again, subclasses must define the function.
          * @param time the time at which to evaluate the derivative
          * @param inState the Matrix to evaluate the derivative of at /a time.
          * @param inStateDot the derivative of /a inState evaluated at /a time.
          * @return a reference to /a inStateDot
          */
      virtual gpstk::Matrix<double>& 
      derivative(long double time,
                 const gpstk::Matrix<double>& inState,
                 gpstk::Matrix<double>& inStateDot) = 0;

         /// Return the current time of the system.
      double getTime(void) 
      { return currentTime; }

         /// Return the current state of the system.
      const Matrix<double>& getState(void) 
      { return currentState; }

   protected:
      
         /// Current time of the system
      double currentTime;

         /// State of the system at the current time
      gpstk::Matrix<double> currentState;

      double teps;   //< Precision for time calculations and comparisons
      int M;         //< Number of rows in the state
      int N;         //< Number of columns in the state

   private:

         /// Disallow copy constructor
         RKIntegrator(const RKIntegrator& cloneDonor);

         /// Disallow the assignment operator
         RKIntegrator& operator= (const RKIntegrator& right);

   }; // end class RKIntegrator

   //@}

} // end namespace gpstk

#endif


