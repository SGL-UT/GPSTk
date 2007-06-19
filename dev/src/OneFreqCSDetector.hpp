
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


    /** This is a class to detect cycle slips using observables in just one frequency.
     * This class is meant to be used with the GNSS data structures objects
     * found in "DataStructures" class.
     *
     * A typical way to use this class follows:
     *
     * @code
     *   RinexObsStream rin("ebre0300.02o");
     *
     *   gnssRinex gRin;
     *   OneFreqCSDetector markCSC1;
     *
     *   while(rin >> gRin) {
     *      gRin >> markCSC1;
     *   }
     * @endcode
     *
     * The "OneFreqCSDetector" object will visit every satellite in the GNSS data
     * structure that is "gRin" and will decide if a cycle slip has happened in the
     * given observable.
     *
     * By default, the algorithm will use C1 and L1 observables, and the LLI1 index.
     * The result (a 0 if a cycle slip is found, 1 otherwise) will be stored in the
     * data structure as the CSL1 index. Note that these data types may be changed
     * using the appropriate methods. For example:
     *
     * @code
     *    markCSC1.setCodeType(TypeID::P2);
     *    markCSC1.setPhaseType(TypeID::L2);
     *    markCSC1.setLLIType(TypeID::LLI2);
     *    markCSC1.setResultType(TypeID::CSI2);
     * @endcode
     *
     * This algorithm will compute the bias between code and phase, and will compare
     * it with a mean bias that is computed on the fly. If the current bias exceeds a
     * given threshold, then a cycle slip is declared. The algorithm will also use
     * the corresponding LLI index (and the RINEX epoch flag, if present) to guide
     * its decision.
     *
     * The threshold, as well as the filter window size and the maximum allowed 
     * time interval between two successive measures, may be tuned with their 
     * corresponding methods. For instance:
     *
     * @code
     *    markCSC1.setMaxNumSigmas(3.5);
     *    markCSC1.setMaxWindowSize(20);
     * @endcode
     *
     * Please be aware that the window size should not be too big, because 
     * other factors (such as the ionospheric drift) may show up in the bias, affecting
     * the algorithm. When using 1 Hz data sampling, a window size between 60 and 100
     * samples will be fine. 
     *
     * When used with the ">>" operator, this class returns the same incoming
     * data structure with the cycle slip index inserted along their corresponding
     * satellites. Be warned that if a given satellite does not have the 
     * observations required, it will be summarily deleted from the data
     * structure.
     *
     */    
    class OneFreqCSDetector
    {
    public:

        /// Default constructor, setting default parameters and C1 and L1 observables.
        OneFreqCSDetector() : codeType(TypeID::C1), phaseType(TypeID::L1), lliType(TypeID::LLI1), resultType(TypeID::CSL1), deltaTMax(31.0), maxWindowSize(60), maxNumSigmas(4.5), defaultBiasSigma(4.0) {};


        /** Common constructor
         *
         * @param codeT         Type of code to be used.
         * @param dtMax         Maximum interval of time allowed between two successive epochs.
         * @param mwSize        Maximum  size of filter window, in samples.
         * @param mnSigmas      Maximum deviation allowed before declaring cycle slip (in number of sigmas).
         * @param dbSigma       Default value assigned to sigma when filter starts.
         */
        OneFreqCSDetector(const TypeID& codeT, const double& dtMax = 31.0, const int& mwSize = 60, const double& mnSigmas = 4.5, const double& dbSigma = 4.0) : codeType(codeT), deltaTMax(dtMax), maxNumSigmas(mnSigmas), defaultBiasSigma(dbSigma)
        {
            // Don't allow window sizes less than 1
            if (mwSize > 1) maxWindowSize = mwSize; else maxWindowSize = 60;

            switch ( codeT.type )
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


        /** Method to set the default code type to be used.
         * @param codeT     TypeID of code to be used
         */
        virtual void setCodeType(const TypeID& codeT)
        {
           codeType = codeT;
        };


        /// Method to get the default code type being used.
        virtual TypeID getCodeType() const
        {
           return codeType;
        };


        /** Method to set the default phase type to be used.
         * @param phaseT    TypeID of phase to be used
         */
        virtual void setPhaseType(const TypeID& phaseT)
        {
           phaseType = phaseT;
        };


        /// Method to get the default phase type being used.
        virtual TypeID getPhaseType() const
        {
           return phaseType;
        };


        /** Method to set the default LLI to be used.
         * @param lliT    LLI to be used
         */
        virtual void setLLIType(const TypeID& lliT)
        {
           lliType = lliT;
        };


        /// Method to get the default LLI being used.
        virtual TypeID getLLIType() const
        {
           return lliType;
        };


        /** Method to set the default return type to be used.
         * @param returnT    TypeID to be returned
         */
        virtual void setResultType(const TypeID& resultT)
        {
           resultType = resultT;
        };


        /// Method to get the default return type being used.
        virtual TypeID getResultType() const
        {
           return resultType;
        };


        /** Method to set the maximum interval of time allowed between two successive epochs.
         * @param maxDelta      Maximum interval of time, in seconds
         */
        virtual void setDeltaTMax(const double& maxDelta)
        {
           deltaTMax = maxDelta;
        };


        /// Method to get the maximum interval of time allowed between two successive epochs.
        virtual double getDeltaTMax() const
        {
           return deltaTMax;
        };


        /** Method to set the maximum size of filter window, in samples.
         * @param maxSize       Maximum size of filter window, in samples.
         */
        virtual void setMaxWindowSize(const int& maxSize)
        {
            // Don't allow window sizes less than 1
            if (maxSize > 1) maxWindowSize = maxSize; else maxWindowSize = 60;
        };


        /// Method to get the maximum size of filter window, in samples.
        virtual int getMaxWindowSize() const
        {
           return maxWindowSize;
        };


        /** Method to set the maximum deviation allowed before declaring cycle slip (in number of sigmas).
         * @param maxNSigmas        Maximum deviation allowed before declaring cycle slip (in number of sigmas).
         */
        virtual void setMaxNumSigmas(const double& maxNSigmas)
        {
           maxNumSigmas = maxNSigmas;
        };


        /// Method to get the maximum deviation allowed before declaring cycle slip (in number of sigmas).
        virtual double getMaxNumSigmas() const
        {
           return maxNumSigmas;
        };


        /** Method to set the default value assigned to sigma when filter starts.
         * @param defSigma      Default value assigned to sigma when filter starts.
         */
        virtual void setDefaultBiasSigma(const double& defSigma)
        {
           defaultBiasSigma = defSigma;
        };


        /// Method to get the default value assigned to sigma when filter starts.
        virtual double getDefaultBiasSigma() const
        {
           return defaultBiasSigma;
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
            // Default constructor initializing the data in the structure
            filterData() : previousEpoch(DayTime::BEGINNING_OF_TIME), windowSize(0), meanBias(0.0), meanSigma2(0.0) {};

            DayTime previousEpoch;  ///< The previous epoch time stamp.
            int windowSize;         ///< The filter window size.
            double meanBias;        ///< Accumulated mean bias (pseudorange - phase).
            double meanSigma2;      ///< Accumulated mean bias sigma squared.
        };


        /// Map holding the information regarding every satellite
        std::map<SatID, filterData> OneFreqData;


        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param epoch     Time of observations.
         * @param sat       SatID.
         * @param tvMap     Data structure of TypeID and values.
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
            deltaT = ( epoch.MJDdate() - OneFreqData[sat].previousEpoch.MJDdate() ) * DayTime::SEC_DAY;

            // Store current epoch as former epoch
            OneFreqData[sat].previousEpoch = epoch;

            bias = code - phase;        // Current value of code-phase bias

            // Increment size of window and check limit
            ++OneFreqData[sat].windowSize;
            if (OneFreqData[sat].windowSize > maxWindowSize) OneFreqData[sat].windowSize = maxWindowSize;

            // Check if receiver already declared cycle slip or too much time has elapsed
            // Note: If tvMap(lliType) doesn't exist, then 0 will be returned and that test will pass
            if ( (epochflag==1) || (epochflag==6) || (tvMap(lliType)!=0.0) || (deltaT > deltaTMax) )
            {
                OneFreqData[sat].windowSize = 1;
            }

            if (OneFreqData[sat].windowSize > 1)
            {
                deltaBias = (bias - OneFreqData[sat].meanBias);

                dif2 = deltaBias*deltaBias;     // Square difference between biases
                thr2 = OneFreqData[sat].meanSigma2 * maxNumSigmas * maxNumSigmas;   // Compute threshold^2

                // If difference in biases is bigger or equal to threshold, then cycle slip
                if (dif2 >= thr2)
                {
                    OneFreqData[sat].windowSize = 1;
                } else {
                    // Update mean bias
                    OneFreqData[sat].meanBias = OneFreqData[sat].meanBias + deltaBias/(static_cast<double>(OneFreqData[sat].windowSize));

                    // Update mean variance
                    OneFreqData[sat].meanSigma2 = OneFreqData[sat].meanSigma2 + ( (dif2-OneFreqData[sat].meanSigma2) ) / (static_cast<double>(OneFreqData[sat].windowSize));
                }
            }

            if (OneFreqData[sat].windowSize <= 1)   // If a cycle-slip happened
            {
                // Set mean bias to current code-phase bias
                OneFreqData[sat].meanBias = bias;

                // Set mean variance to default variance
                OneFreqData[sat].meanSigma2 = defaultBiasSigma * defaultBiasSigma;
                reportCS = true;
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
