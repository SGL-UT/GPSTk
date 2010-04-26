#pragma ident "$Id: $"

/**
 * @file CiraExponentialDrag.hpp
 * Computes the acceleration due to drag on a satellite
 * using an exponential Earth atmosphere model.
 */

#ifndef   GPSTK_CIRA_EXPONENTIAL_DRAG_HPP
#define   GPSTK_CIRA_EXPONENTIAL_DRAG_HPP


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


#include "AtmosphericDrag.hpp"

namespace gpstk
{
      /** @addtogroup GeoDynamics */
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




