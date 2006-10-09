
/**
 * @file ExtractD2.hpp
 * This class eases D2 data extraction from a RinexObsData object.
 */

#ifndef ExtractD2_GPSTK
#define ExtractD2_GPSTK

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


    /// This class eases D2 data extraction from a RinexObsData object.
    class ExtractD2 : public ExtractData
    {
    public:

        /// Default constructor
        ExtractD2() throw(InvalidData)
        {
            valid = false; 
            checkData = false;  // This is not code, and we don't want to check these values
        };


        /** Pull out the D2 observation from a RinexObsData object
         * @param rinexData     The Rinex data set holding the observations
         *
         * @return
         *  Number of satellites with D2 data available
         */
        inline virtual int getData(const RinexObsData& rinexData) throw(InvalidData)
        {
            return ExtractData::getData(rinexData, RinexObsHeader::D2);
        };


        /// Destructor
        inline virtual ~ExtractD2() {};


    protected:
        bool valid;         // true only if results are valid

   }; // end class ExtractData
   

   //@}
   
}

#endif
