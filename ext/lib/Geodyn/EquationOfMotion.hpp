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
 * @file EquationOfMotion.hpp
 *
 * Class to provides the mechanism for passing a method
 * that computes the derivatives to an integrator.
 */

#ifndef GPSTK_EQUATION_OF_MOTION_HPP
#define GPSTK_EQUATION_OF_MOTION_HPP

#include "Vector.hpp"

namespace gpstk
{
      /// @ingroup GeoDynamics 
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
