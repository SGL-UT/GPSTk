
/**
 * @file ExtractP2.hpp
 * This class eases P2 data extraction from a RinexObsData object.
 */

#ifndef ExtractP2_GPSTK
#define ExtractP2_GPSTK

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


    /// This class eases P2 data extraction from a RinexObsData object.
    class ExtractP2 : public ExtractData
    {
    public:

        /// Default constructor
        ExtractP2() throw(InvalidData)
        {
            valid = false; 
            checkData = true;
        };


        /** Pull out the P2 observation from a RinexObsData object
         * @param rinexData     The Rinex data set holding the observations
         *
         * @return
         *  Number of satellites with P2 data available
         */
        inline virtual int getData(const RinexObsData& rinexData) throw(InvalidData)
        {
            return ExtractData::getData(rinexData, RinexObsHeader::P2);
        };


        /// Destructor
        inline virtual ~ExtractP2() {};


    protected:
        bool valid;         // true only if results are valid

   }; // end class ExtractData
   

   //@}
   
}

#endif
