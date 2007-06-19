
/**
 * @file LICSDetector.hpp
 * This is a class to detect cycle slips using LI observables.
 */

#ifndef LICSDETECTOR_GPSTK
#define LICSDETECTOR_GPSTK

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



#include "DataStructures.hpp"



namespace gpstk
{

    /** @addtogroup GPSsolutions */
    //@{


    /** This is a class to detect cycle slips using LI observables.
     * This class is meant to be used with the GNSS data structures objects
     * found in "DataStructures" class.
     *
     * A typical way to use this class follows:
     *
     * @code
     *   RinexObsStream rin("ebre0300.02o");
     *
     *   gnssRinex gRin;
     *   ComputeLI getLI;
     *   LICSDetector markCSLI;
     *
     *   while(rin >> gRin) {
     *      gRin >> getLI >> markCSLI;
     *   }
     * @endcode
     *
     * The "LICSDetector" object will visit every satellite in the GNSS data
     * structure that is "gRin" and will decide if a cycle slip has happened in the
     * given observable.
     *
     * The algorithm will use LI observables, and the LLI1 and LLI2 indexes.
     * The result (a 0 if a cycle slip is found, 1 otherwise) will be stored in the
     * data structure both as the CSL1 and CSL2 indexes.
     *
     * This algorithm will use some values as maximum interval of time between
     * two successive epochs, minimum threshold for declaring cycle slip and LI
     * combination limit drift.
     * 
     * The default values are usually fine, but nevertheless you may change them 
     * with the appropriate methods. The former is of special importance for the
     * maximum interval time, that should be adjusted to your sampling rate. By
     * default it is 61 seconds, adapted to 30 seconds per sample RINEX files.
     *
     * When used with the ">>" operator, this class returns the same incoming
     * data structure with the cycle slip indexes inserted along their corresponding
     * satellites. Be warned that if a given satellite does not have the 
     * observations required, it will be summarily deleted from the data
     * structure.
     *
     */    
    class LICSDetector
    {
    public:

        /// Default constructor, setting default parameters.
        LICSDetector() : obsType(TypeID::LI), lliType1(TypeID::LLI1), lliType2(TypeID::LLI2), resultType1(TypeID::CSL1), resultType2(TypeID::CSL2), deltaTMax(61.0), minThreshold(0.04), LIDrift(0.002) {};


        /** Common constructor
         *
         * @param mThr          Minimum threshold for declaring cycle slip, in meters.
         * @param drift         LI combination limit drift, in meters/second.
         * @param dtMax         Maximum interval of time allowed between two successive epochs, in seconds.
         */
        LICSDetector(const double& mThr, const double& drift, const double& dtMax = 61.0) : obsType(TypeID::LI), lliType1(TypeID::LLI1), lliType2(TypeID::LLI2), resultType1(TypeID::CSL1), resultType2(TypeID::CSL2)
        {
            setDeltaTMax(dtMax);
            setMinThreshold(mThr);
            setLIDrift(drift);
        };


        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param epoch     Time of observations.
         * @param gData     Data object holding the data.
         * @param epochflag Epoch flag.
         */
        virtual satTypeValueMap& Detect(const DayTime& epoch, satTypeValueMap& gData, const short& epochflag=0)
        {
            double value1(0.0);
            double lli1(0.0);
            double lli2(0.0);

            SatIDSet satRejectedSet;

            // Loop through all the satellites
            satTypeValueMap::iterator it;
            for (it = gData.begin(); it != gData.end(); ++it) 
            {
                try
                {
                    // Try to extract the values
                    value1 = (*it).second(obsType);
                }
                catch(...)
                {
                    // If some value is missing, then schedule this satellite for removal
                    satRejectedSet.insert( (*it).first );
                    continue;
                }
                try
                {
                    // Try to get the LLI1 index
                    lli1  = (*it).second(lliType1);
                }
                catch(...)
                {
                    // If LLI #1 is not found, set it to zero
                    // You REALLY want to have BOTH LLI indexes properly set
                    lli1 = 0.0;
                }
                try
                {
                    // Try to get the LLI2 index
                    lli2  = (*it).second(lliType2);
                }
                catch(...)
                {
                    // If LLI #2 is not found, set it to zero
                    // You REALLY want to have BOTH LLI indexes properly set
                    lli2 = 0.0;
                }
                // If everything is OK, then get the new values inside the structure
                // This way of doing it allows concatenation of several different cycle slip detectors
                (*it).second[resultType1] += getDetection(epoch, (*it).first, (*it).second, epochflag, value1, lli1, lli2);
                if ( (*it).second[resultType1] > 1.0 ) (*it).second[resultType1] = 1.0;
                // We will mark both cycle slip flags
                (*it).second[resultType2] = (*it).second[resultType1];
            }
            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;
        };


        /** Method to set the maximum interval of time allowed between two successive epochs.
         * @param maxDelta      Maximum interval of time, in seconds
         */
        virtual void setDeltaTMax(const double& maxDelta)
        {
            // Don't allow delta times less than or equal to 0
            if (maxDelta > 0.0) deltaTMax = maxDelta; else deltaTMax = 61.0;
        };


        /// Method to get the maximum interval of time allowed between two successive epochs, in seconds.
        virtual double getDeltaTMax() const
        {
           return deltaTMax;
        };


        /** Method to set the minimum threshold for cycle slip detection, in meters.
         * @param mThr      Minimum threshold for cycle slip detection, in meters.
         */
        virtual void setMinThreshold(const double& mThr)
        {
            // Don't allow thresholds less than 0
            if (mThr < 0.0) minThreshold = 0.04; else minThreshold = mThr;
        };


        /// Method to get the minimum threshold for cycle slip detection, in meters.
        virtual double getMinThreshold() const
        {
           return minThreshold;
        };


        /** Method to set the LI combination limit drift, in meters/second
         * @param drift     LI combination limit drift, in meters/second.
         */
        virtual void setLIDrift(const double& drift)
        {
            // Don't allow drift less than or equal to 0
            if (drift > 0.0) LIDrift = drift; else LIDrift = 0.002;
        };


        /// Method to get the minimum threshold for cycle slip detection, in meters.
        virtual double getLIDrift() const
        {
           return LIDrift;
        };


        /** Returns a gnnsSatTypeValue object, adding the new data generated when calling this object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssSatTypeValue& Detect(gnssSatTypeValue& gData)
        {
            (*this).Detect(gData.header.epoch, gData.body);
            return gData;
        };


        /** Returns a gnnsRinex object, adding the new data generated when calling this object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssRinex& Detect(gnssRinex& gData)
        {
            (*this).Detect(gData.header.epoch, gData.body, gData.header.epochFlag);
            return gData;
        };


        /// Destructor
        virtual ~LICSDetector() {};


    private:

        /// Type of code.
        TypeID obsType;

        /// Type of LLI1 record.
        TypeID lliType1;

        /// Type of LLI2 record.
        TypeID lliType2;

        /// Type of result #1.
        TypeID resultType1;

        /// Type of result #2.
        TypeID resultType2;


        /// Maximum interval of time allowed between two successive epochs, in seconds.
        double deltaTMax;


        /// Minimum threshold for declaring cycle slip, in meters.
        double minThreshold;


        /// LI combination limit drift, in meters/second.
        double LIDrift;


        /// A structure used to store filter data for a SV.
        struct filterData
        {
            // Default constructor initializing the data in the structure
            filterData() : formerEpoch(DayTime::BEGINNING_OF_TIME), windowSize(0), formerLI(0.0), formerBias(0.0), formerDeltaT(1.0) {};

            DayTime formerEpoch;    ///< The previous epoch time stamp.
            int windowSize;         ///< Size of current window, in samples.
            double formerLI;        ///< Value of the previous LI observable.
            double formerBias;      ///< Previous bias (LI_1 - LI_0).
            double formerDeltaT;    ///< Previous time difference, in seconds.
        };


        /// Map holding the information regarding every satellite
        std::map<SatID, filterData> LIData;


        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param epoch     Time of observations.
         * @param sat       SatID.
         * @param tvMap     Data structure of TypeID and values.
         * @param epochflag Epoch flag.
         * @param li        Current LI observation value.
         * @param lli1      LLI1 index.
         * @param lli2      LLI2 index.
         */
        virtual double getDetection(const DayTime& epoch, const SatID& sat, typeValueMap& tvMap, const short& epochflag, const double& li, const double& lli1, const double& lli2)
        {
            bool reportCS(false);
            double currentDeltaT(0.0); // Difference between current and former epochs, in sec
            double currentBias(0.0);   // Difference between current and former LI values
            double deltaLimit(0.0);    // Limit to declare cycle slip
            double delta(0.0);
            double tempLLI1(0.0);
            double tempLLI2(0.0);


            // Get the difference between current epoch and former epoch, in seconds
            currentDeltaT = ( epoch.MJDdate() - LIData[sat].formerEpoch.MJDdate() ) * DayTime::SEC_DAY;

            // Store current epoch as former epoch
            LIData[sat].formerEpoch = epoch;

            currentBias = li - LIData[sat].formerLI;   // Current value of LI difference

            // Increment size of window and check if first time here
            ++LIData[sat].windowSize;

            // Check if receiver already declared cycle slip or too much time has elapsed
            // Note: If tvMap(lliType1) or tvMap(lliType2) don't exist, then 0 will be returned and those tests will pass
            if ( (tvMap(lliType1)==1.0) || (tvMap(lliType1)==3.0) || (tvMap(lliType1)==5.0) || (tvMap(lliType1)==7.0) ) tempLLI1 = 1.0;

            if ( (tvMap(lliType2)==1.0) || (tvMap(lliType2)==3.0) || (tvMap(lliType2)==5.0) || (tvMap(lliType2)==7.0) ) tempLLI2 = 1.0;

            if ( (epochflag==1) || (epochflag==6) || (tempLLI1==1.0) || (tempLLI2==1.0) || (currentDeltaT > deltaTMax) )
            {
                LIData[sat].windowSize = 0;
                reportCS = true;
            }

            if (LIData[sat].windowSize > 1)
            {
                deltaLimit = minThreshold + std::abs(LIDrift*currentDeltaT);
                // Compute a linear interpolation and compute LI_predicted - LI_current
                delta = std::abs(currentBias - LIData[sat].formerBias*currentDeltaT/LIData[sat].formerDeltaT);

                if (delta > deltaLimit) reportCS = true;
            }

            // Let's prepare for the next time
            LIData[sat].formerLI = li;
            LIData[sat].formerBias = currentBias;
            LIData[sat].formerDeltaT = currentDeltaT;               

            if (reportCS) return 1.0; else return 0.0;
        };

   }; // end class LICSDetector
   

    /// Input operator from gnssSatTypeValue to LICSDetector.
    inline gnssSatTypeValue& operator>>(gnssSatTypeValue& gData, LICSDetector& liD)
    {
            liD.Detect(gData);
            return gData;
    }


    /// Input operator from gnssRinex to LICSDetector.
    inline gnssRinex& operator>>(gnssRinex& gData, LICSDetector& liD)
    {
            liD.Detect(gData);
            return gData;
    }

   



   //@}
   
}

#endif
