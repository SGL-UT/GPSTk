
/**
 * @file MWCSDetector.hpp
 * This is a class to detect cycle slips using the Melbourne-Wubbena combination.
 */

#ifndef MWCSDETECTOR_GPSTK
#define MWCSDETECTOR_GPSTK

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
#include <list>


namespace gpstk
{

    /** @addtogroup GPSsolutions */
    //@{


    /** This is a class to detect cycle slips using MW observables.
     * This class is meant to be used with the GNSS data structures objects
     * found in "DataStructures" class.
     *
     * A typical way to use this class follows:
     *
     * @code
     *   RinexObsStream rin("ebre0300.02o");
     *
     *   gnssRinex gRin;
     *   ComputeMelbourneWubbena getMW;
     *   MWCSDetector markCSMW;
     *
     *   while(rin >> gRin) {
     *      gRin >> getMW >> markCSMW;
     *   }
     * @endcode
     *
     * The "MWCSDetector" object will visit every satellite in the GNSS data
     * structure that is "gRin" and will decide if a cycle slip has happened in the
     * given observable.
     *
     * The algorithm will use MW observables, and the LLI1 and LLI2 indexes.
     * The result (a 0 if a cycle slip is found, 1 otherwise) will be stored in the
     * data structure both as the CSL1 and CSL2 indexes.
     *
     * In taking the decision, this algorithm will use criteria as the maximum
     * interval of time between two successive epochs and the maximum number of 
     * Melbourne-Wubbena wavelenghts allowed above or below the MW combination 
     * average for that arc.
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
     * You should be aware that the Melbourne-Wubbena combination is based on a
     * mix of code and phase observations, and it is very noisy. Therefore, it 
     * has a tendency to yield a fair number of false positives if you are not
     * careful with its parameters. Because of this, the default parameters are
     * very conservative, i.e., the detector is NOT much sensitive.
     *
     * Best results are achieved when using this detector as a "backup" detector
     * for detectors based in LI combination, like this:
     *
     * @code
     *   RinexObsStream rin("ebre0300.02o");
     *
     *   gnssRinex gRin;
     *   ComputeLI getLI;
     *   LICSDetector markCSLI;
     *   ComputeMelbourneWubbena getMW;
     *   MWCSDetector markCSMW;
     *
     *   while(rin >> gRin) {
     *      gRin >> getLI >> getMW >> markCSLI >> markCSMW;
     *   }
     * @endcode
     *
     * @sa LICSDetector.hpp for more information.
     *
     * \warning Cycle slip detectors are objets that store their internal state,
     * so you MUST NOT use the SAME object to process DIFFERENT data streams.
     *
     */    
    class MWCSDetector
    {
    public:

        /// Default constructor, setting default parameters.
        MWCSDetector() : obsType(TypeID::MWubbena), lliType1(TypeID::LLI1), lliType2(TypeID::LLI2), resultType1(TypeID::CSL1), resultType2(TypeID::CSL2), deltaTMax(61.0), maxNumLambdas(10.0), useLLI(true) {};


        /** Common constructor
         *
         * @param mLambdas      Maximum deviation allowed before declaring cycle slip (in number of Melbourne-Wubbena wavelenghts).
         * @param dtMax         Maximum interval of time allowed between two successive epochs, in seconds.
         */
        MWCSDetector(const double& mLambdas, const double& dtMax = 61.0, const bool& use = true) : obsType(TypeID::MWubbena), lliType1(TypeID::LLI1), lliType2(TypeID::LLI2), resultType1(TypeID::CSL1), resultType2(TypeID::CSL2), useLLI(use)
        {
            setDeltaTMax(dtMax);
            setMaxNumLambdas(mLambdas);
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
                if (useLLI)
                {
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


        /** Method to set the maximum deviation allowed before declaring cycle slip (in number of Melbourne-Wubbena wavelenghts).
         * @param mLambdas     Maximum deviation allowed before declaring cycle slip (in number of Melbourne-Wubbena wavelenghts).
         */
        virtual void setMaxNumLambdas(const double& mLambdas)
        {
            // Don't allow number of lambdas less than or equal to 0
            if (mLambdas > 0.0) maxNumLambdas = mLambdas; else maxNumLambdas = 10.0;
        };


        /// Method to get the maximum deviation allowed before declaring cycle slip (in number of Melbourne-Wubbena wavelenghts).
        virtual double getMaxNumLambdas() const
        {
           return maxNumLambdas;
        };


        /** Method to set whether the LLI indexes will be used as an aid or not.
         * @param use   Boolean value enabling/disabling LLI check
         */
        virtual void setUseLLI(const bool& use)
        {
            useLLI = use;
        };


        /// Method to know if the LLI check is enabled or disabled.
        virtual bool getUseLLI() const
        {
           return useLLI;
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
        virtual ~MWCSDetector() {};



    private:

        /// Type of code.
        TypeID obsType;

        /// Type of LMW1 record.
        TypeID lliType1;

        /// Type of LMW2 record.
        TypeID lliType2;

        /// Type of result #1.
        TypeID resultType1;

        /// Type of result #2.
        TypeID resultType2;


        /// Maximum interval of time allowed between two successive epochs, in seconds.
        double deltaTMax;


        /// Maximum deviation allowed before declaring cycle slip (in number of Melbourne-Wubbena wavelenghts).
        double maxNumLambdas;


        /// This field tells whether to use or ignore the LLI indexes as an aid. 
        bool useLLI;


        /// A structure used to store filter data for a SV.
        struct filterData
        {
            // Default constructor initializing the data in the structure
            filterData() : formerEpoch(DayTime::BEGINNING_OF_TIME), windowSize(0), meanMW(0.0) {};

            DayTime formerEpoch;    ///< The previous epoch time stamp.
            int windowSize;         ///< Size of current window, in samples.
            double meanMW;          ///< Accumulated mean value of combination
        };

        /// Map holding the information regarding every satellite
        std::map<SatID, filterData> MWData;



        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param epoch     Time of observations.
         * @param sat       SatID.
         * @param tvMap     Data structure of TypeID and values.
         * @param epochflag Epoch flag.
         * @param mw        Current MW observation value.
         * @param lli1      LLI1 index.
         * @param lli2      LLI2 index.
         */
        virtual double getDetection(const DayTime& epoch, const SatID& sat, typeValueMap& tvMap, const short& epochflag, const double& mw, const double& lli1, const double& lli2)
        {
            bool reportCS(false);
            double currentDeltaT(0.0);  // Difference between current and former epochs, in sec
            double currentBias(0.0);    // Difference between current and former MW values
            double lambdaLimit(maxNumLambdas*0.862);    // Limit to declare cycle slip based on lambdas (LambdaLW = 0.862 m)
            double tempLLI1(0.0);
            double tempLLI2(0.0);


            // Get the difference between current epoch and former epoch, in seconds
            currentDeltaT = ( epoch.MJDdate() - MWData[sat].formerEpoch.MJDdate() ) * DayTime::SEC_DAY;

            // Store current epoch as former epoch
            MWData[sat].formerEpoch = epoch;

            // Difference between current value of MW and average value
            currentBias = std::abs(mw - MWData[sat].meanMW);

            // Increment size of window
            ++MWData[sat].windowSize;

            // Check if receiver already declared cycle slip or too much time has elapsed
            // Note: If tvMap(lliType1) or tvMap(lliType2) don't exist, then 0 will be used and those tests will pass
            if ( (tvMap(lliType1)==1.0) || (tvMap(lliType1)==3.0) || (tvMap(lliType1)==5.0) || (tvMap(lliType1)==7.0) ) tempLLI1 = 1.0;

            if ( (tvMap(lliType2)==1.0) || (tvMap(lliType2)==3.0) || (tvMap(lliType2)==5.0) || (tvMap(lliType2)==7.0) ) tempLLI2 = 1.0;

            if ( (epochflag==1) || (epochflag==6) || (tempLLI1==1.0) || (tempLLI2==1.0) || (currentDeltaT > deltaTMax) )
            {
                MWData[sat].windowSize = 1;     // We reset the filter with this
                reportCS = true;                // Report cycle slip
            }


            if (MWData[sat].windowSize > 1)
            {
                // Test for current bias bigger than lambda limit and for current bias squared bigger than sigma squared limit
                if ( (currentBias > lambdaLimit) )
                {
                    MWData[sat].windowSize = 1;     // We reset the filter with this
                    reportCS = true;                // Report cycle slip
                }
            }

            // Let's prepare for the next time
            // If a cycle-slip happened or just starting up
            if (MWData[sat].windowSize < 2)
            {
                MWData[sat].meanMW = mw;
            } else {
                // Compute average
                MWData[sat].meanMW += (mw - MWData[sat].meanMW) / (static_cast<double>(MWData[sat].windowSize));
            }

            if (reportCS) return 1.0; else return 0.0;
        };

   }; // end class MWCSDetector
   

    /// Input operator from gnssSatTypeValue to MWCSDetector.
    inline gnssSatTypeValue& operator>>(gnssSatTypeValue& gData, MWCSDetector& mwD)
    {
            mwD.Detect(gData);
            return gData;
    }


    /// Input operator from gnssRinex to MWCSDetector.
    inline gnssRinex& operator>>(gnssRinex& gData, MWCSDetector& mwD)
    {
            mwD.Detect(gData);
            return gData;
    }

   



   //@}
   
}

#endif
