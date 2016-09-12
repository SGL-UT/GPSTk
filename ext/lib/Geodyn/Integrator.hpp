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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
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
 * @file Integrator.hpp
 *
 * This is an abstract base class for objects solving
 * a ODE system with integrator
 */


#ifndef GPSTK_INTEGRATOR_HPP
#define GPSTK_INTEGRATOR_HPP

#include "Vector.hpp"
#include "Matrix.hpp"
#include "EquationOfMotion.hpp"

namespace gpstk
{
      /// @ingroup GeoDynamics 
      //@{

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

      // @}

}  // End of namespace 'gpstk'

#endif  // GPSTK_INTEGRATOR_HPP
