#pragma ident "$Id: $"

/**
 * @file EquationOfMotion.hpp
 *
 * Class to provides the mechanism for passing a method
 * that computes the derivatives to an integrator.
 */

#ifndef GPSTK_EQUATION_OF_MOTION_HPP
#define GPSTK_EQUATION_OF_MOTION_HPP

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

namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /**
       * The EquationsOfMotion provides the mechanism for passing a method
       * that computes the derivatives to an integrator.
       *
       */
   class EquationOfMotion
   {
   public:
         /// Default constructor
      EquationOfMotion()
      {}
      
         /// Default deconstructor
      virtual ~EquationOfMotion()
      {}

         /** Compute the derivatives.
          * @params t    time or the independent variable.
          * @params y    the required data.
          * @return      the derivatives.
          */
      virtual Vector<double> getDerivatives(const double& t, const Vector<double>& y) = 0;


   }; // End of class 'EquationOfMotion'

      // @}

}  // End of namespace 'gpstk'

#endif  // GPSTK_EQUATION_OF_MOTION_HPP




