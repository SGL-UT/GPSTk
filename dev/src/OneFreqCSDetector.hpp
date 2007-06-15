
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007
//
//============================================================================



#include "DataStructures.hpp"



namespace gpstk
{

    /** @addtogroup GPSsolutions */
    //@{


    /// This is a class to detect cycle slips using observables in just one frequency.
    class OneFreqCSDetector
    {
    public:

        /// Default constructor, setting default parameters and C1 and L1 observables.
        OneFreqCSDetector() : codeType(TypeID::C1), phaseType(TypeID::L1), lliType(TypeID::LLI1), resultType(TypeID::CSL1), deltaTMax(31.0), maxWindowSize(60), maxNumSigmas(4.5), defaultBiasSigma(4.0) {};


        /** Common constructor
         *
         * @param codeType      Type of code to be used.
         * @param dtMax         Maximum interval of time allowed between two successive epochs.
         * @param mwSize        Maximum  size of filter window, in samples.
         * @param mnSigmas      Maximum deviation allowed before declaring cycle slip (in number of sigmas).
         * @param dbSigma       Default value assigned to sigma when filter starts.
         */
        OneFreqCSDetector(const TypeID& codeT, const double& dtMax = 31.0, const int& mwSize = 60, const double& mnSigmas = 4.5, const double& dbSigma = 4.0) : codeType(codeT), deltaTMax(dtMax), maxWindowSize(mwSize), maxNumSigmas(mnSigmas), defaultBiasSigma(dbSigma)
        {
            switch ( codeType.type )
            {
                case TypeID::C1:
                    phaseType   = TypeID::L1;
                    lliType     = TypeID::LLI1;
                    resultType  = TypeID::CSL1;
                    break;
                case TypeID::C2:
                    phaseType   = TypeID::L2;
                    lliType     = TypeID::LLI2;
                    resultType  = TypeID::CSL2;
                    break;
                case TypeID::C5:
                    phaseType   = TypeID::L5;
                    lliType     = TypeID::LLI5;
                    resultType  = TypeID::CSL5;
                    break;
                case TypeID::C6:
                    phaseType   = TypeID::L6;
                    lliType     = TypeID::LLI6;
                    resultType  = TypeID::CSL6;
                    break;
                case TypeID::C7:
                    phaseType   = TypeID::L7;
                    lliType     = TypeID::LLI7;
                    resultType  = TypeID::CSL7;
                    break;
                case TypeID::C8:
                    phaseType   = TypeID::L8;
                    lliType     = TypeID::LLI8;
                    resultType  = TypeID::CSL8;
                    break;
                default:
                    phaseType   = TypeID::L1;
                    lliType     = TypeID::LLI1;
                    resultType  = TypeID::CSL1;
                };
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
            double value2(0.0);

            SatIDSet satRejectedSet;

            // Loop through all the satellites
            satTypeValueMap::iterator it;
            for (it = gData.begin(); it != gData.end(); ++it) 
            {
                try
                {
                    // Try to extract the values
                    value1 = (*it).second(codeType);
                    value2 = (*it).second(phaseType);
                }
                catch(...)
                {
                    // If some value is missing, then schedule this satellite for removal
                    satRejectedSet.insert( (*it).first );
                    continue;
                }
                // If everything is OK, then get the new value inside the structure
                (*it).second[resultType] = getDetection(epoch, (*it).first, (*it).second, epochflag, value1, value2);
            }
            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;
        };


// *** ME FALTAN METODOS SET/GET XXX

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
        virtual ~OneFreqCSDetector() {};


    private:

        /// Type of code.
        TypeID codeType;

        /// Type of phase.
        TypeID phaseType;

        /// Type of LLI record.
        TypeID lliType;

        /// Type of result.
        TypeID resultType;

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
            int windowSize;         ///< The filter window size.
            double meanBias;        ///< Accumulated mean bias (pseudorange - phase).
            double meanSigma2;      ///< Accumulated mean bias sigma squared.
            bool csDetected;        ///< Whether a cycle slip was detected
        };


        std::map<SatID, filterData> OneFreqData;


        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param epoch     Time of observations.
         * @param gData     Data object holding the data.
         * @param epochflag Epoch flag.
         * @param code      Current code observation value.
         * @param phase     Current phase observation value.
         */
        virtual double getDetection(const DayTime& epoch, const SatID& sat, typeValueMap& tvMap, const short& epochflag, const double& code, const double& phase)
        {
            bool reportCS(false);
            double deltaT(0.0); // Difference between current and former epochs, in sec
            double bias(0.0);   // Code-phase bias
            double dif2(0.0);   // Difference between biases, squared.
            double thr2(0.0);   // Threshold in sigmas, squared.
            double deltaBias(0.0);  // Difference between biases

            // Get the difference between current epoch and former epoch, in seconds
            if (OneFreqData[sat].windowSize > 1) deltaT = (epoch.MJDdate() - OneFreqData[sat].previousEpoch.MJDdate())*DayTime::SEC_DAY;

            // Store current epoch as former epoch
            OneFreqData[sat].previousEpoch = epoch;

            bias = code - phase;        // Current value of code-phase bias

            // Increment size of window and check limit
            ++OneFreqData[sat].windowSize;
            if (OneFreqData[sat].windowSize > maxWindowSize) OneFreqData[sat].windowSize = maxWindowSize;

            double temp = tvMap(lliType);

            // Check if receiver already declared cycle slip or too much time has elapsed
            if ( (epochflag==1) || (epochflag==6) || (temp==1.0) || (temp==3.0) || (deltaT > deltaTMax) )
            {
                OneFreqData[sat].windowSize = 1;
                reportCS = true;
            }

            if (OneFreqData[sat].windowSize > 1)
            {
                deltaBias = (bias - OneFreqData[sat].meanBias);
                dif2 = deltaBias*deltaBias;     // Square difference between biases

                // Compute threshold^2
                thr2 = OneFreqData[sat].meanSigma2 * maxNumSigmas * maxNumSigmas;

                // If difference in biases is bigger or equal to threshold, then cycle slip
                if (dif2 >= thr2)
                {
                    OneFreqData[sat].windowSize = 1;
                    reportCS = true;
                }

                // If there was no cycle slip, prepare for next iteration
                if (!(reportCS))
                {
                    // Update mean bias
                    OneFreqData[sat].meanBias = OneFreqData[sat].meanBias + deltaBias/(static_cast<double>(OneFreqData[sat].windowSize));

                    // Update mean variance
                    OneFreqData[sat].meanSigma2 = OneFreqData[sat].meanSigma2 + ( (dif2-OneFreqData[sat].meanSigma2) ) / (static_cast<double>(OneFreqData[sat].windowSize));
                }

            } else {    // If windowSize <= 1

                // Set mean bias to current code-phase bias
                OneFreqData[sat].meanBias = bias;

                // Set mean variance to default variance
                OneFreqData[sat].meanSigma2 = defaultBiasSigma * defaultBiasSigma;
            }

            if (reportCS) return 1.0; else return 0.0;
        };

   }; // end class OneFreqCSDetector
   

    /// Input operator from gnssSatTypeValue to OneFreqCSDetector.
    inline gnssSatTypeValue& operator>>(gnssSatTypeValue& gData, OneFreqCSDetector& oneF)
    {
            oneF.Detect(gData);
            return gData;
    }


    /// Input operator from gnssRinex to OneFreqCSDetector.
    inline gnssRinex& operator>>(gnssRinex& gData, OneFreqCSDetector& oneF)
    {
            oneF.Detect(gData);
            return gData;
    }

   



   //@}
   
}

#endif
