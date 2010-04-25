#pragma ident "$Id: $"

/**
 * @file Integrator.hpp
 *
 * This is an abstract base class for objects solving
 * a ODE system with integrator
 */


#ifndef GPSTK_INTEGRATOR_HPP
#define GPSTK_INTEGRATOR_HPP

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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================


#include "Vector.hpp"
#include "Matrix.hpp"
#include "EquationOfMotion.hpp"

namespace gpstk
{
      /** This is an abstract base class for objects solving
       * a ODE system with integrator
       */
   class Integrator
   {
   public:

         /// Default constructor
      Integrator() : stepSize(1.0)
      {}


         /// Default deconstructor
      virtual ~Integrator()
      {}


         /** Take a single integration step.
          * @param t     tindependent variable (usually the time)
          * @param y     inputs (usually the state)
          * @param peom  Object containing the Equations of Motion
          * @param tf    next time
          * @return      containing the new state
          */
      virtual Vector<double> integrateTo(const double&           t, 
                                         const Vector<double>&   y, 
                                         EquationOfMotion*       peom,
                                         const double&           tf ) = 0;

         /// get step size
      double getStepSize() const
      { return stepSize; }


         /// set step size
      void setStepSize(const double& step)
      { stepSize = step; }

   protected:

         /// step size
      double stepSize;

   }; // End of class 'Integrator'

}  // End of namespace 'gpstk'

#endif  // GPSTK_INTEGRATOR_HPP
