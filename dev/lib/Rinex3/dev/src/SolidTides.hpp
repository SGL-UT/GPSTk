#pragma ident "$Id: SolidTides.hpp 1161 2008-03-27 17:16:22Z ckiesch $"

/**
 * @file SolidTides.hpp
 * Computes the effect of solid Earth tides at a given position and epoch.
 */

#ifndef SOLIDTIDES_HPP
#define SOLIDTIDES_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007
//
//============================================================================

 
#include <cmath>
#include <string>

#include "DayTime.hpp"
#include "Triple.hpp"
#include "Position.hpp"
#include "icd_200_constants.hpp"
#include "AstronomicalFunctions.hpp"
#include "SunPosition.hpp"
#include "MoonPosition.hpp"



namespace gpstk
{
      /** @addtogroup GPSsolutions */
      //@{
   
      /** This class computes the effect of solid Earth tides at a given
       * position and epoch.
       *
       * The model used is the simple quadrupole response model described
       * by J.G. Williams (1970).
       *
       * Given the limitations of the algorithms used to compute the Sun 
       * and Moon positions, this class is limited to the period between 
       * March 1st, 1900 and February 28th, 2100.
       *
       */
   class SolidTides
   {
   public:

         /// Default constructor
      SolidTides() throw() {}

         /// Destructor
      virtual ~SolidTides() {}


         /** Returns the effect of solid Earth tides (meters) at the given
          * position and epoch, in the Up-East-North (UEN) reference frame.
          *
          * @param[in] t Epoch to look up
          * @param[in] p Position of interest
          *
          * @return a Triple with the solid tidal effect, in meters and in
          * the UEN reference frame.
          *
          * @throw InvalidRequest If the request can not be completed for any
          * reason, this is thrown. The text may have additional information
          * as to why the request failed.
          */
         Triple getSolidTide(const DayTime& t, 
                            const Position& p) const 
            throw(InvalidRequest);


   private:

         /// Love numbers
      static const double H_LOVE, L_LOVE;

         /// Phase lag
      static const double PH_LAG;


   }; // end class SolidTides


   //@}
   
} // namespace gpstk
#endif  // SOLIDTIDES_HPP
