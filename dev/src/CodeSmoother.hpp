
/**
 * @file CodeSmoother.hpp
 * This class smoothes a given code observable using the corresponding phase observable.
 */

#ifndef CODE_SMOOTHER_GPSTK
#define CODE_SMOOTHER_GPSTK

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

    /** @addtogroup DataStructures */
    //@{


    /** This class smoothes a given code observable using the corresponding phase observable.
     * This class is meant to be used with the GNSS data structures objects
     * found in "DataStructures" class.
     *
     * A typical way to use this class follows:
     *
     * @code
     *   RinexObsStream rin("ebre0300.02o");
     *
     *   gnssRinex gRin;
     *   OneFreqCSDetector markCSC1;    // We MUST mark cycle slips
     *   CodeSmoother smoothC1;
     *
     *   while(rin >> gRin) {
     *      gRin >> markCSC1 >> smoothC1;
     *   }
     * @endcode
     *
     * The "CodeSmoother" object will visit every satellite in the GNSS data
     * structure that is "gRin" and will smooth the given code observation using
     * the corresponding phase observation.
     *
     * By default, the algorithm will use C1 and L1 observables, and the CSL1 index
     * will be consulted for cycle slip information. You can change these settings
     * with the appropriate set methods.
     *
     * When used with the ">>" operator, this class returns the same incoming
     * data structure with the code observation smoothed (unless the resultType
     * field is changed). Be warned that if a given satellite does not have the 
     * observations required, it will be summarily deleted from the data
     * structure.
     *
     * Another important parameter is the maxWindowSize field. By default, it is
     * set to 100 samples (you may adjust that with the setMaxWindowSize() method).
     *
     * A window of 100 samples is typical and appropriate when working with data 
     * sampled at 1 Hz, because then the full window will last at most 100 seconds.
     *
     * However, if for instance your samples are taken at 30 seconds (and you are
     * working with C1/L1 or other ionosphere-affected observation pair), then a 
     * window of 50 minutes will be used and you will get badly distorted data 
     * because of ionosphere drift, among other effects.
     *
     * A good rule here is to make sure that the filter window lasts at most 5 minutes.
     * Therefore, for a 30 s sampling data set you should set your smoother object like
     * this:
     *
     * @code
     *   CodeSmoother smoothC1;
     *   smoothC1.setMaxWindowSize(8);
     * @endcode
     *
     * Resulting in a 4 minutes filter window.
     *
     * \warning Code smoothers are objets that store their internal state,
     * so you MUST NOT use the SAME object to process DIFFERENT data streams.
     *
     */
    class CodeSmoother
    {
    public:

        /// Default constructor, setting default parameters and C1 and L1 as observables.
        CodeSmoother() : codeType(TypeID::C1), phaseType(TypeID::L1), resultType(TypeID::C1), maxWindowSize(100), csFlag(TypeID::CSL1) { };


        /** Common constructor
         *
         * @param codeT         Type of code to be smoothed.
         * @param mwSize        Maximum  size of filter window, in samples.
         */
        CodeSmoother(const TypeID& codeT, const int& mwSize = 100) : codeType(codeT)
        {
            // Don't allow window sizes less than 1
            if (mwSize > 1) maxWindowSize = mwSize; else maxWindowSize = 1;

            switch ( codeT.type )
            {
                case TypeID::C1:
                    phaseType   = TypeID::L1;
                    csFlag      = TypeID::CSL1;
                    resultType  = TypeID::C1;
                    break;
                case TypeID::C2:
                    phaseType   = TypeID::L2;
                    csFlag      = TypeID::CSL2;
                    resultType  = TypeID::C2;
                    break;
                case TypeID::P1:
                    phaseType   = TypeID::L1;
                    csFlag      = TypeID::CSL1;
                    resultType  = TypeID::P1;
                    break;
                case TypeID::P2:
                    phaseType   = TypeID::L2;
                    csFlag      = TypeID::CSL2;
                    resultType  = TypeID::P2;
                    break;
                case TypeID::C5:
                    phaseType   = TypeID::L5;
                    csFlag      = TypeID::CSL5;
                    resultType  = TypeID::C5;
                    break;
                case TypeID::C6:
                    phaseType   = TypeID::L6;
                    csFlag      = TypeID::CSL6;
                    resultType  = TypeID::C6;
                    break;
                case TypeID::C7:
                    phaseType   = TypeID::L7;
                    csFlag      = TypeID::CSL7;
                    resultType  = TypeID::C7;
                    break;
                case TypeID::C8:
                    phaseType   = TypeID::L8;
                    csFlag      = TypeID::CSL8;
                    resultType  = TypeID::C8;
                    break;
                default:
                    phaseType   = TypeID::L1;
                    csFlag      = TypeID::CSL1;
                    resultType  = TypeID::C1;
                };
        };


        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Smooth(satTypeValueMap& gData)
        {
            double codeObs(0.0);
            double phaseObs(0.0);
            double flagObs(0.0);

            SatIDSet satRejectedSet;

            // Loop through all the satellites
            satTypeValueMap::iterator it;
            for (it = gData.begin(); it != gData.end(); ++it) 
            {
                try
                {
                    // Try to extract the values
                    codeObs  = (*it).second(codeType);
                    phaseObs = (*it).second(phaseType);
                    flagObs  = (*it).second(csFlag);
                }
                catch(...)
                {
                    // If some value is missing, then schedule this satellite for removal
                    satRejectedSet.insert( (*it).first );
                    continue;
                }
                // If everything is OK, then process according if there is a cycle slip or not.
                (*it).second[resultType] = getSmoothing((*it).first, codeObs, phaseObs, flagObs);
            }
            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;
        };


        /** Returns a gnnsSatTypeValue object, adding the new data generated when calling this object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssSatTypeValue& Smooth(gnssSatTypeValue& gData)
        {
            (*this).Smooth(gData.body);
            return gData;
        };


        /** Returns a gnnsRinex object, adding the new data generated when calling this object.
         *
         * @param gData    Data object holding the data.
         */
        virtual gnssRinex& Smooth(gnssRinex& gData)
        {
            (*this).Smooth(gData.body);
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


        /** Method to set the default cycle slip type to be used.
         * @param csT   Cycle slip type to be used
         */
        virtual void setCSFlag(const TypeID& csT)
        {
           csFlag = csT;
        };


        /// Method to get the default cycle slip type being used.
        virtual TypeID getCSFlag() const
        {
           return csFlag;
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


        /** Method to set the maximum size of filter window, in samples.
         * @param maxSize       Maximum size of filter window, in samples.
         */
        virtual void setMaxWindowSize(const int& maxSize)
        {
            // Don't allow window sizes less than 1
            if (maxSize > 1) maxWindowSize = maxSize; else maxWindowSize = 1;
        };


        /// Method to get the maximum size of filter window, in samples.
        virtual int getMaxWindowSize() const
        {
           return maxWindowSize;
        };


        /// Destructor
        virtual ~CodeSmoother() {};


    private:

        /// Type of code observation to be used.
        TypeID codeType;


        /// Type of phase observation to be used.
        TypeID phaseType;


        /// Type assigned to the resulting smoothed code.
        TypeID resultType;


        /// Maximum size of filter window, in samples.
        int maxWindowSize;


        /// Cycle slip flag. It MUST be present. @sa OneFreqCSDetector.hpp class.
        TypeID csFlag;


        /// A structure used to store filter data for a SV.
        struct filterData
        {
            // Default constructor initializing the data in the structure
            filterData() : windowSize(1), previousCode(0.0), previousPhase(0.0) {};

            int windowSize;         ///< The filter window size.
            double previousCode;        ///< Accumulated mean bias (pseudorange - phase).
            double previousPhase;      ///< Accumulated mean bias sigma squared.
        };


        /// Map holding the information regarding every satellite
        std::map<SatID, filterData> SmoothingData;



        /// Compute the combination of observables.
        virtual double getSmoothing(const SatID& sat, const double& code, const double& phase, const double& flag)
        {
            if ( flag!=0.0 )  // In case we have a cycle slip
            {
                // Prepare the structure for the next iteration
                SmoothingData[sat].previousCode = code;
                SmoothingData[sat].previousPhase = phase;
                SmoothingData[sat].windowSize = 1;
                return code;    // We don't need any further processing
            }
            
            // In case we didn't have cycle slip
            double smoothedCode(0.0);

            // Increment size of window and check limit
            ++SmoothingData[sat].windowSize;
            if (SmoothingData[sat].windowSize > maxWindowSize) SmoothingData[sat].windowSize = maxWindowSize;

            // The formula used is the following:
            //
            // CSn = (1/n)*Cn + ((n-1)/n)*(CSn-1 + Ln - Ln-1)
            //
            // As window size "n" increases, the former formula gives more
            // weight to the previous smoothed code CSn-1 plus the phase bias
            // (Ln - Ln-1), and less weight to the current code observation Cn
            smoothedCode = ( code + ((static_cast<double>(SmoothingData[sat].windowSize)) - 1.0) * (SmoothingData[sat].previousCode + (phase - SmoothingData[sat].previousPhase) ) ) / (static_cast<double>(SmoothingData[sat].windowSize));

            // Store results for next iteration
            SmoothingData[sat].previousCode = smoothedCode;
            SmoothingData[sat].previousPhase = phase;

            return smoothedCode;
        };


   }; // end class CodeSmoother


    /// Input operator from gnssSatTypeValue to CodeSmoother.
    inline gnssSatTypeValue& operator>>(gnssSatTypeValue& gData, CodeSmoother& codeS)
    {
            codeS.Smooth(gData);
            return gData;
    }


    /// Input operator from gnssRinex to CodeSmoother.
    inline gnssRinex& operator>>(gnssRinex& gData, CodeSmoother& codeS)
    {
            codeS.Smooth(gData);
            return gData;
    }

   

   //@}
   
}

#endif
