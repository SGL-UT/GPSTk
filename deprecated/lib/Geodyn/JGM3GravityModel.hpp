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
 * @file JGM3GravityModel.hpp
 * JGM3 gravity model
 */

#ifndef GPSTK_JGM3_GRAVITY_MODEL_HPP
#define GPSTK_JGM3_GRAVITY_MODEL_HPP

#include "SphericalHarmonicGravity.hpp"

namespace gpstk
{
      /// @ingroup GeoDynamics 
      //@{

      /** JGM3 gravity model
       */
   class JGM3GravityModel : public SphericalHarmonicGravity
   {
   public:
         /// Constructor
      JGM3GravityModel(int n = 20, int m = 20) 
         : SphericalHarmonicGravity(n, m)
      {
         gmData.modelName = "JGM3";

         gmData.GM = 3.98600441500e+14;
         gmData.refDistance = 6378136.300000;

         gmData.includesPermTide = true;

         gmData.refMJD = 46431.0;
         gmData.dotC20 = 0.0000000000E-11;
         gmData.dotC21 = 0.0000000000E-11;
         gmData.dotS21 = 0.0000000000E-11;

         gmData.maxDegree = 70;
         gmData.maxOrder = 70;

         gmData.unnormalizedCS.resize(71, 71);
         gmData.unnormalizedCS = &gmcs[0][0];

      };

         /// Default destructor
      virtual ~JGM3GravityModel(){};


      virtual void initialize(){};

   private:

         /**Gravitational coefficients C, S are efficiently stored in a single array CS. The lower 
          * triangle matrix CS holds the non-sectorial C coefficients C[n][m] ( n != m ). Sectorial C 
          * coefficients C[n][n] are the diagonal elements of CS and the upper triangular matrix 
          * stores the S[n][m] ( m != 0 ) coefficients in columns, for the same degree n. Mapping of 
          * CS to C, S is achieved through C[n][m] = CS[n][m], S[n][m] = CS[m-1][n].
          */
      static const double gmcs[71][71];      // it's unnormalized

   }; // End of class 'JGM3'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_JGM3_GRAVITY_MODEL_HPP
