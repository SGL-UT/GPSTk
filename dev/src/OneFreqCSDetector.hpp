
/**
 * @file OneFreqCSDetector.hpp
 * This is a class to detect cycle slips using observables in just one frequency.
 */

#ifndef ONEFREQCSDETECTOR_GPSTK
#define ONEFREQCSDETECTOR_GPSTK

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


#include <map>

#include "SatID.hpp"
#include "DayTime.hpp"
#include "DataStructures.hpp"
#include "RinexObsData.hpp"


namespace gpstk
{

    /** @addtogroup GPSsolutions */
    //@{


    /// This is a class to detect cycle slips using observables in just one frequency.
    class OneFreqCSDetector
    {
    public:

        /// Maximum interval of time allowed between two successive epochs.
        double deltaTMax;


        /// Maximum size of filter window, in samples.
        int maxWindowSize;


        /// Maximum deviation allowed before declaring cycle slip (in number of sigmas).
        double maxNumSigmas;


        /// Default value assigned to sigma when filter starts.
        double defaultBiasSigma;

        /// A structure used to store filter data for a SV.
        struct filterData
        {
            DayTime previousEpoch;  ///< The previous epoch time stamp.
            int window Size;        ///< The filter window size.
            double meanBias;        ///< Accumulated mean bias (pseudorange - phase).
            double meanSigma2;      ///< Accumulated mean bias sigma squared.
            bool csDetected;        ///< Whether a cycle slip was detected
        };


        typedef std::map<SatID, filterData> OneFreqSatFilterData;


        /// Default constructor
        OneFreqCSDetector();

        /// Method to detect cycle-slips
        bool detect(RinexObsData &rinexData);

        /// Method to detect cycle-slips
        bool detect(RinexOneTypeData &codeObs, RinexOneTypeData &phaseObs);

        /// Method to ask if a given SV had cycle-slip. You must first run "detect".
        bool hasCS(SatID &sv);


   }; // end class OneFreqCSDetector
   

   //@}
   
}

#endif
