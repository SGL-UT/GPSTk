#pragma ident "$Id$"

/**
 * @file CheckPRData.hpp
 * This class checks that pseudorange data is between reasonable values.
 */

#ifndef CheckPRData_GPSTK
#define CheckPRData_GPSTK

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
//  Dagoberto Salazar - gAGE. 2006
//
//============================================================================



namespace gpstk
{

    /** @addtogroup RinexObs */
    //@{


    /// This class checks that pseudorange data is between reasonable values.
    class CheckPRData
    {
    public:

        /// Default constructor
        CheckPRData() : minPRange(15000000.0), maxPRange(30000000.0) {};


        /// Constructor that allows to set the data span values
        CheckPRData(const double& min, const double& max) : minPRange(min), maxPRange(max) {};


        /** Checks that the given pseudorange data is between the limits 
         * @param prange    The pseudorange data to be tested
         *
         * @return
         *  True if check was OK.
         */
        virtual bool check(const double& prange) const
        {
            return ( prange>=minPRange && prange<=maxPRange );
        };  // end CheckPRData::check()


        /// Set the minimum pseudorange value allowed for data (in meters).
        virtual void setMinPRange(const double& minPR) { minPRange = minPR; };

        /// Get the minimum pseudorange value allowed for data (in meters).
        virtual double getMinPRange(void) const { return minPRange; };

        /// Set the maximum pseudorange value allowed for data (in meters).
        virtual void setMaxPRange(const double& maxPR) { maxPRange = maxPR; };

        /// Get the maximum pseudorange value allowed for data (in meters).
        virtual double getMaxPRange(void) const { return maxPRange; };


        /// Destructor
        virtual ~CheckPRData() {};


    protected:

        /// Minimum pseudorange value allowed for input data (in meters).
        double minPRange;

        /// Maximum pseudorange value allowed for input data (in meters).
        double maxPRange;



   }; // end class CheckPRData
   

   //@}
   
}

#endif
