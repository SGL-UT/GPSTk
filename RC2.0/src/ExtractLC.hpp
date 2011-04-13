#pragma ident "$Id$"

/**
 * @file ExtractLC.hpp
 * This class eases LC combination data extraction from a RinexObsData object.
 */

#ifndef ExtractLC_GPSTK
#define ExtractLC_GPSTK

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



#include "ExtractCombinationData.hpp"
#include "icd_200_constants.hpp"


namespace gpstk
{

    /** @addtogroup RinexObs */
    //@{


    /// This class eases LC combination data extraction from a RinexObsData object.
    class ExtractLC : public ExtractCombinationData
    {
    public:

        /// Default constructor
        ExtractLC() throw(InvalidData) : typeObs1(RinexObsHeader::L1), typeObs2(RinexObsHeader::L2)
        {
            valid = false;
            checkData = false;  // This is not code, and we don't want to check these values
        };


        /** Compute the LC observation from a RinexObsData object (in meters)
         * @param rinexData     The Rinex data set holding the observations
         *
         * @return
         *  Number of satellites with LC combination data available
         */
        virtual int getData(const RinexObsData& rinexData) throw(InvalidData)
        {
            return ExtractCombinationData::getData(rinexData, typeObs1, typeObs2);
        };  // end ExtractLC::getData()


        /// Destructor
        virtual ~ExtractLC() {};


    protected:
        // Compute the combination of observables.
        virtual double getCombination(double obs1, double obs2) throw(InvalidData)
        {
            return ( (GAMMA_GPS*obs1*L1_WAVELENGTH - obs2*L2_WAVELENGTH)/(GAMMA_GPS - 1.0) );
        };


    private:
        RinexObsHeader::RinexObsType typeObs1;
        RinexObsHeader::RinexObsType typeObs2;


   }; // end class ExtractLC
   

   //@}
   
}

#endif
