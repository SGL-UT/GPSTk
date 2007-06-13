
/**
 * @file SimpleIURAWeight.hpp
 * Class to assign weights to satellites based on their URA Index (IURA).
 */

#ifndef SIMPLEIURAWEIGHT_BASE_GPSTK
#define SIMPLEIURAWEIGHT_BASE_GPSTK

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



#include "WeightBase.hpp"
#include "BCEphemerisStore.hpp"
#include "TabularEphemerisStore.hpp"
#include "EngEphemeris.hpp"
#include "RinexObsHeader.hpp"
#include "icd_200_constants.hpp"
#include <vector>


namespace gpstk
{

    /** @addtogroup GPSsolutions */
    //@{

    /**
     * Class to assign weights to satellites based on their URA Index (IURA).
     */
    class SimpleIURAWeight : public WeightBase
    {
    public:

        /// Empty constructor
        SimpleIURAWeight(void) { valid = false; };


        /** Compute and return a vector with the weights for the given satellites
         * @param time           Epoch weights will be computed for
         * @param Satellites     Vector of satellites
         * @param bcEph          Satellite broadcast ephemeris
         * 
         * @return
         *  Number of satellites with valid weights
         *
         * \note
         * Method isValid() will return false if some satellite does not have a
         * valid weight. Also, its PRN will be set to a negative value.
         *
         */
        virtual int getWeights(DayTime& time, Vector<SatID>& Satellites, BCEphemerisStore& bcEph) throw(InvalidWeights);


        /** Compute and return a vector with the weights for the given satellites
         * @param time           Epoch weights will be computed for
         * @param Satellites     Vector of satellites
         * @param preciseEph     Satellite precise ephemeris
         * 
         * @return
         *  Number of satellites with valid weights
         *
         * \note
         * Method isValid() will return false if some satellite does not have a
         * valid weight. Also, its PRN will be set to a negative value.
         *
         * \note
         * This method assigns an URA of 0.1 m to all satellites.
         *
         */
        virtual int getWeights(DayTime& time, Vector<SatID>& Satellites, TabularEphemerisStore& preciseEph) throw(InvalidWeights);


        /// Vector of weights for these satellites
        Vector<double> weightsVector;

        /// Vector with the PRN of satellites with weights available for computing.
        Vector<SatID> availableSV;

        /// Vector with the PRN of satellites rejected or with no proper weights.
        Vector<SatID> rejectedSV;

        /// Return validity of weights
        virtual bool isValid(void)
            { return valid; }


   }; // end class SimpleIURAWeight
   

   //@}
   
}

#endif
