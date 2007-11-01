
/**
 * @file AstronomicalFunctions.hpp
 * Useful functions used in astronomical computations.
 */

#ifndef GPSTK_ASTRONOMICALFUNCTIONS_HPP
#define GPSTK_ASTRONOMICALFUNCTIONS_HPP

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
#include "Xvt.hpp"
#include "icd_200_constants.hpp"


namespace gpstk
{
    /** @addtogroup ephemcalc */
    //@{
   

    /** Function to change from CIS to CTS(ECEF) coordinate system (coordinates in meters)
     * @param posCis    Coordinates in CIS system (in meters).
     * @param t         Epoch
     */
    Xvt CIS2CTS(const Xvt posCIS, const DayTime& t);


    /** Function to convert from UTC to sidereal time
     * @param t         Epoch
     *
     * @return sidereal time in hours
     */
    double UTC2SID(const DayTime& t);

   //@}
   
} // namespace gpstk
#endif  // GPSTK_ASTRONOMICALFUNCTIONS_HPP
