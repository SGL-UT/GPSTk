
/**
 * @file ExtractCombinationData.hpp
 * This is the base class to ease extraction of a combination of data from a RinexObsData object.
 */

#ifndef Extract_CombinationData_GPSTK
#define Extract_CombinationData_GPSTK

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


    /// This class eases the extraction of a combination of data from a RinexObsData object.
    class ExtractCombinationData : public ExtractData
    {
    public:

        /// Default constructor
        ExtractCombinationData() throw(InvalidData)
        {
            valid = false;
            checkData = true;
        };


        /** Pull out the combination of observations from a RinexObsData object
         * @param rinexData     The Rinex data set holding the observations
         * @param typeObs1      The #1 type of observation we want to get
         * @param typeObs2      The #2 type of observation we want to get
         *
         * @return
         *  Number of satellites with this combination of observable data available
         */
        virtual int getData(const RinexObsData& rinexData, RinexObsHeader::RinexObsType typeObs1, RinexObsHeader::RinexObsType typeObs2) throw(InvalidData)
        {
        try {
            // Let's make sure each time we start with clean Vectors
            availableSV.resize(0);
            obsData.resize(0);

            // Create a CheckPRData object with the given limits
            CheckPRData checker(minPRange, maxPRange);

            // Let's define the "it" iterator to visit the observations PRN map
            // RinexSatMap is a map from SatID to RinexObsTypeMap: 
            //      std::map<SatID, RinexObsTypeMap>
            RinexObsData::RinexSatMap::const_iterator it;
            for (it = rinexData.obs.begin(); it!= rinexData.obs.end(); it++) 
            {
                // RinexObsTypeMap is a map from RinexObsType to RinexDatum:
                //   std::map<RinexObsHeader::RinexObsType, RinexDatum>
                RinexObsData::RinexObsTypeMap otmap;
                // Let's define a iterator to visit the observations type map
                RinexObsData::RinexObsTypeMap::const_iterator itObs1;
                // The "second" field of a RinexSatMap (it) is a RinexObsTypeMap (otmap)
                otmap = (*it).second;

                // Let's find the observation type inside the RinexObsTypeMap that is "otmap"
                itObs1 = otmap.find(typeObs1);

                // Let's check if we found this type of observation
                if (itObs1!=otmap.end())
                {
                    // Find an itObs2 observation inside the RinexObsTypeMap that is "otmap"
                    // Let's define a iterator to visit the observations type map
                    RinexObsData::RinexObsTypeMap::const_iterator itObs2;
                    itObs2 = otmap.find(typeObs2);
                    // If we indeed found a typeObs2 observation, let's compute the combination
                    if (itObs2!=otmap.end()) 
                    {
                        // The "second" part of a RinexObsTypeMap is a RinexDatum, whose public
                        // attribute "data" indeed holds the actual numerical data
                        double combinationValue = getCombination((*itObs1).second.data, (*itObs2).second.data);

                        // Let's check that the combination is between the limits
                        if (checker.check(combinationValue) || !(checkData) ) 
                        {
                            // Store all relevant data of this epoch
                            availableSV = availableSV && (*it).first;
                            obsData = obsData && combinationValue;
                            // Let's use for the resutl the lli and ssi values corresponding to the first observable
                            tempDatum = (*itObs1).second;
                            tempDatum.data = combinationValue;
                            extractedData.obs[(*it).first] = tempDatum;
                        }
                    }
                }
            } // End of data extraction from this epoch
        }
        catch(...) {
            InvalidData e("Unable to get combination data from RinexObsData object");
            GPSTK_THROW(e);
        }

        // Let's record the number of SV with this type of data available
        numSV = (int)obsData.size();
        extractedData.numSvs = numSV;

        // Fill the remaining files of extractedData object
        extractedData.epochFlag = rinexData.epochFlag;
        extractedData.time = rinexData.time;
        // Note: extractedData observation type should be set manually
        extractedData.typeObs = RinexObsHeader::UN;


        // If everything is fine so far, then the results should be valid
        valid = true;

        return numSV;

        };  // end ExtractCombinationData::getData()


        /// Destructor
        virtual ~ExtractCombinationData() {};


    protected:
       /// Compute the combination of observables. You must define this method according to your specific combination.
        virtual double getCombination(double obs1, double obs2) throw(InvalidData) = 0;

        RinexDatum tempDatum;


   }; // end class ExtractCombinationData
   

   //@}
   
}

#endif
