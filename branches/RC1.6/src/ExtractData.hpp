#pragma ident "$Id$"

/**
 * @file ExtractData.hpp
 * This is the base class to ease data extraction from a RinexObsData object.
 */

#ifndef ExtractData_GPSTK
#define ExtractData_GPSTK

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



#include "Exception.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "CheckPRData.hpp"
#include "Vector.hpp"


namespace gpstk
{
    /// Thrown when some problem appeared when extracting data
    /// @ingroup exceptiongroup
    NEW_EXCEPTION_CLASS(InvalidData, gpstk::Exception);


    /** @addtogroup RinexObs */
    //@{


    /// This is the base class to ease data extraction from a RinexObsData object.
    class ExtractData
    {
    public:

        /// Return validity of data
        inline bool isValid(void)
            { return valid; }


        /// Number of satellites with available data
        int numSV;


        /// Vector with the PRN of satellites with available data.
        Vector<SatID> availableSV;


        /// Vector holding the available data
        Vector<double> obsData;


        /// Default constructor
        ExtractData() throw(InvalidData) : checkData(true), valid(false), minPRange(15000000.0), maxPRange(30000000.0) {};


        /** Pull out the selected observation type from a RinexObsData object
         * @param rinexData     The Rinex data set holding the observations
         * @param typeObs       The type of observation we want to get
         *
         * @return
         *  Number of satellites with this kind of data available
         */
        inline virtual int getData(const RinexObsData& rinexData, RinexObsHeader::RinexObsType typeObs) throw(InvalidData)
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
                itObs1 = otmap.find(typeObs);

                // Let's check if we found this type of observation and it is between the limits
                if ( (itObs1!=otmap.end()) && ( (checker.check((*itObs1).second.data)) || !(checkData) ) )
                {
                    // Store all relevant data of this epoch
                    availableSV = availableSV && (*it).first;
                    obsData = obsData && (*itObs1).second.data;
                }
            } // End of data extraction from this epoch
        }
        catch(...) {
            InvalidData e("Unable to get data from RinexObsData object");
            GPSTK_THROW(e);
        }

        // Let's record the number of SV with this type of data available
        numSV = (int)obsData.size();

        // If everything is fine so far, then the results should be valid
        valid = true;

        return numSV;

        };  // end ExtractData::getData()


        /// Set this to true if you want to enable data checking within given boundaries (default for code measurements)
        bool checkData;

        /// Set the minimum pseudorange value allowed for data (in meters).
        virtual void setMinPRange(const double minPR) { minPRange = minPR; };

        /// Get the minimum pseudorange value allowed for data (in meters).
        virtual double getMinPRange(void) { return minPRange; };

        /// Set the maximum pseudorange value allowed for data (in meters).
        virtual void setMaxPRange(const double maxPR) { maxPRange = maxPR; };

        /// Get the minimum pseudorange value allowed for data (in meters).
        virtual double getMaxPRange(void) { return maxPRange; };


        /// Destructor
        inline virtual ~ExtractData() {};


    protected:
        /// True only if results are valid
        bool valid;

        /// Minimum pseudorange value allowed for input data (in meters).
        double minPRange;

        /// Maximum pseudorange value allowed for input data (in meters).
        double maxPRange;


   }; // end class ExtractData
   

   //@}
   
}

#endif
