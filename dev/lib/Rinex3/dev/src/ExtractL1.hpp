
/**
 * @file ExtractL1.hpp
 * This class eases L1 data extraction from a RinexObsData object.
 */

#ifndef ExtractL1_GPSTK
#define ExtractL1_GPSTK

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



#include "ExtractData.hpp"


namespace gpstk
{

    /** @addtogroup RinexObs */
    //@{


    /// This class eases L1 data extraction from a RinexObsData object.
    class ExtractL1 : public ExtractData
    {
    public:

        /// Default constructor
        ExtractL1() throw(InvalidData) : valid(false) 
        {
            checkData = false;  // This is not code, and we don't want to check these values
        };


        /** Pull out the L1 observation from a RinexObsData object (in cycles)
         * @param rinexData     The Rinex data set holding the observations
         *
         * @return
         *  Number of satellites with L1 data available
         */
        inline virtual int getData(const RinexObsData& rinexData) throw(InvalidData)
        {
            return ExtractData::getData(rinexData, RinexObsHeader::L1);
        };


        /// Destructor
        inline virtual ~ExtractL1() {};


    protected:
        bool valid;         // true only if results are valid

   }; // end class ExtractData
   

   //@}
   
}

#endif
