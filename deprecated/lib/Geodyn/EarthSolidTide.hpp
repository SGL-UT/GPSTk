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
* @file EarthSolidTide.hpp
* Class to do Earth Solid Tide correction
*/

#ifndef GPSTK_SOLID_TIDE_HPP
#define GPSTK_SOLID_TIDE_HPP

namespace gpstk
{
      /// @ingroup GeoDynamics 
      //@{

      /**Class to do Earth Solid Tide correction
       * reference: IERS Conventions 2003
       */
   class EarthSolidTide
   {
   public:

         /// Default constructor
      EarthSolidTide(){}

         /// Default destructor
      ~EarthSolidTide(){}

         /**
          * Solid tide to normalized earth potential coefficients
          *
          * @param mjdUtc  UTC in MJD
          * @param dC      correction to normalized coefficients dC
          * @param dS      correction to normalized coefficients dS
          */
      void getSolidTide(double mjdUtc, double dC[], double dS[] );


      void test();

   protected:

         ///  Legendre polynomial
         //  relevant formula can be found in "satellite orbits"(3.23,3.24,3.25 in chapter 3.2.4)
      static double legendrePoly(int n,int m,double u);

      double normFactor (int n, int m) ;
      
         /// Objects to hold parameters
      static const double Argu_C20[21][7];
      static const double Argu_C21[48][7];
      static const double Argu_C22[2][6];
      
   }; // End of class 'EarthSolidTide'

      // @}

}  // End of namespace 'gpstk'

#endif   //GPSTK_SOLID_TIDE_HPP
