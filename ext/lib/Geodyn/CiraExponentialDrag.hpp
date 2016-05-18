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
 * @file CiraExponentialDrag.hpp
 * Computes the acceleration due to drag on a satellite
 * using an exponential Earth atmosphere model.
 */

#ifndef   GPSTK_CIRA_EXPONENTIAL_DRAG_HPP
#define   GPSTK_CIRA_EXPONENTIAL_DRAG_HPP

#include "AtmosphericDrag.hpp"

namespace gpstk
{
      /// @ingroup GeoDynamics 
      //@{

      /**
       * This class computes the acceleration due to drag on a satellite
       * using an exponential Earth atmosphere model. The min altitude is
       * currently 200 km. To go lower, just need to add more values from 
       * the table.
       *
       * This Model is checked on Sep 28th,2009, OK!!!
       *
       * Reference: Vallado, Table 8-4.
       */
   class CiraExponentialDrag : public AtmosphericDrag
   {
   public:
         /// Default construtor
      CiraExponentialDrag() {};

         /// Default destructor
      virtual ~CiraExponentialDrag() {};

      void test();

      
         /** Compute the atmospheric density using an exponential atmosphere model.
          * @param utc Time reference object.
          * @param rb  Reference body object.
          * @param r   ECI position vector in meters.
          * @param v   ECI velocity vector in m/s
          * @return Atmospheric density in kg/m^3.
          */
      virtual double computeDensity(UTCTime utc, 
                                    EarthBody& rb, 
                                    Vector<double> r, 
                                    Vector<double> v);

   protected:

      int brack;

   }; // End of class 'CiraExponentialDrag'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_CIRA_EXPONENTIAL_DRAG_HPP
