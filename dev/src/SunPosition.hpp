
/**
 * @file SunPosition.hpp
 * Returns the approximate position of the Sun at the given epoch in the 
 * ECEF system.
 */

#ifndef GPSTK_SUNPOSITION_HPP
#define GPSTK_SUNPOSITION_HPP

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
#include "AstronomicalFunctions.hpp"



namespace gpstk
{
    /** @addtogroup ephemcalc */
    //@{
   
    /** This class computes the approximate position of the Sun at the given 
     * epoch in the ECEF system. It yields best results between March 1st 1900
     * and February 28th 2100.
     *
     * This is a C++ implementation version based on the FORTRAN version 
     * originally written by P.T. Wallace, Starlink Project. The FORTRAN
     * version of Starlink project was available under the GPL license.
     *
     * More information may be found in http://starlink.jach.hawaii.edu/
     */
    class SunPosition
    {
    public:

        /// Default constructor
        SunPosition() throw() {}

        /// Destructur
        virtual ~SunPosition() {}


        /// Returns the position of Sun ECEF coordinates (meters) at the indicated time.
        /// @param[in] t the time to look up
        /// @return the Xvt of the Sun at time
        /// @throw InvalidRequest If the request can not be completed for any
        ///    reason, this is thrown. The text may have additional
        ///    information as to why the request failed.
        /// @warning This method yields and approximate result, given that pole movement
        /// is not taken into account, neither precession nor nutation.
        Xvt getXvt(const DayTime& t) const throw(InvalidRequest);
      

        /** Function to compute Sun position in CIS system (coordinates in meters)
         * @param t Epoch
         */
        Xvt getXvtCIS(const DayTime& t) const throw(InvalidRequest);


        /// Determine the earliest time for which this object can successfully 
        /// determine the Xvt for the Sun.
        /// @return The initial time
        /// @throw InvalidRequest This is thrown if the object has no data.
        DayTime getInitialTime() const throw(InvalidRequest)
        { return initialTime; }


        /// Determine the latest time for which this object can successfully 
        /// determine the Xvt for the Sun.
        /// @return The final time
        /// @throw InvalidRequest This is thrown if the object has no data.
        DayTime getFinalTime() const throw(InvalidRequest)
        { return finalTime; }


    private:

        /// Time of the first valid time
        static const DayTime initialTime;

        /// Time of the last valid time
        static const DayTime finalTime;

    }; // end class SunPosition


   //@}
   
} // namespace gpstk
#endif  // GPSTK_SUNPOSITION_HPP
