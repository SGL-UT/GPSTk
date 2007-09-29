
/**
 * @file PCSmoother.hpp
 * This class smoothes PC code observables using the corresponding LC phase observable.
 */

#ifndef PC_SMOOTHER_GPSTK
#define PC_SMOOTHER_GPSTK

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
//  Dagoberto Salazar - gAGE ( http:// www.gage.es ). 2007
//
//============================================================================



#include "CodeSmoother.hpp"


namespace gpstk
{

    /** @addtogroup DataStructures */
    //@{


    /** This class smoothes the PC code observable using the corresponding LC phase observable.
     * This class is meant to be used with the GNSS data structures objects
     * found in "DataStructures" class.
     *
     * A typical way to use this class follows:
     *
     * @code
     *   RinexObsStream rin("ebre0300.02o");
     *
     *   gnssRinex gRin;
     *   OneFreqCSDetector markCSL1;    // We MUST mark at least some cycle slips
     *   PCSmoother smoothPC;
     *
     *   while(rin >> gRin) {
     *      gRin >> markCSL1 >> smoothPC;
     *   }
     * @endcode
     *
     * The "PCSmoother" object will visit every satellite in the GNSS data
     * structure that is "gRin" and will smooth the PC code observation using
     * the corresponding LC phase observation.
     *
     * By default, the algorithm will check both the CSL1 and CSL2 index for cycle 
     * slip information. You can change these settings in the constructor and with 
     * the appropriate set methods.
     *
     * When used with the ">>" operator, this class returns the same incoming
     * data structure with the PC code observation smoothed (unless the resultType
     * field is changed). Be warned that if a given satellite does not have the 
     * observations required, it will be summarily deleted from the data
     * structure.
     *
     * Another important parameter is the maxWindowSize field. By default, it is
     * set to 100 samples. You may adjust that with the setMaxWindowSize() method:
     *
     * @code
     *   PCSmoother smoothPC;
     *   smoothPC.setMaxWindowSize(35);
     * @endcode
     *
     * A window of 100 samples is typical and appropriate when working with data 
     * sampled at 1 Hz. Note that the PC observable doesn't suffer the effect of 
     * the ionosphere drift.
     *
     * @sa CodeSmoother.hpp for base class.
     *
     * \warning Code smoothers are objets that store their internal state,
     * so you MUST NOT use the SAME object to process DIFFERENT data streams.
     *
     */
    class PCSmoother : public CodeSmoother
    {
    public:

        /// Default constructor, setting default parameters and C1 and L1 as observables.
        PCSmoother() : codeType(TypeID::PC), phaseType(TypeID::LC), resultType(TypeID::PC), maxWindowSize(100), csFlag1(TypeID::CSL1), csFlag2(TypeID::CSL2)
        {
            setIndex();
        };


        /** Common constructor
         *
         * @param mwSize        Maximum  size of filter window, in samples.
         * @param resultT       TypeID where results will be stored.
         */
        PCSmoother(const int& mwSize, const TypeID& resultT = TypeID::PC) : codeType(TypeID::PC), phaseType(TypeID::LC), resultType(resultT), maxWindowSize(mwSize), csFlag1(TypeID::CSL1), csFlag2(TypeID::CSL2)
        {
            setIndex();
        };


        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Process(satTypeValueMap& gData)
        {
            double codeObs(0.0);
            double phaseObs(0.0);
            double flagObs1(0.0);
            double flagObs2(0.0);

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
                }
                catch(...)
                {
                    // If some value is missing, then schedule this satellite for removal
                    satRejectedSet.insert( (*it).first );
                    continue;
                }
                try
                {
                    // Try to get the first cycle slip flag
                    flagObs1  = (*it).second(csFlag1);
                }
                catch(...)
                {
                    // If flag #1 is not found, no cycle slip is assumed
                    // You REALLY want to have BOTH CS flags properly set
                    flagObs1 = 0.0;
                }
                try
                {
                    // Try to get the second cycle slip flag
                    flagObs2  = (*it).second(csFlag2);
                }
                catch(...)
                {
                    // If flag #2 is not found, no cycle slip is assumed
                    // You REALLY want to have BOTH CS flags properly set
                    flagObs2 = 0.0;
                }
                // Get the smoothed PC.
                (*it).second[resultType] = getSmoothing((*it).first, codeObs, phaseObs, flagObs1, flagObs2);
            }
            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;
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


        /** Method to set the default cycle slip type #1 to be used.
         * @param csT   Cycle slip type to be used
         */
        virtual void setCSFlag1(const TypeID& csT)
        {
           csFlag1 = csT;
        };


        /// Method to get the default cycle slip type #1 being used.
        virtual TypeID getCSFlag1() const
        {
           return csFlag1;
        };


        /** Method to set the default cycle slip type #2 to be used.
         * @param csT   Cycle slip type to be used
         */
        virtual void setCSFlag2(const TypeID& csT)
        {
           csFlag2 = csT;
        };


        /// Method to get the default cycle slip type #2 being used.
        virtual TypeID getCSFlag2() const
        {
           return csFlag2;
        };


        /// Returns an index identifying this object.
        virtual int getIndex(void) const;


        /// Returns a string identifying this object.
        virtual std::string getClassName(void) const;


        /** Sets the index to a given arbitrary value. Use with caution.
         *
         * @param newindex      New integer index to be assigned to current object.
         */
        void setIndex(const int newindex) { (*this).index = newindex; };


        /// Destructor
        virtual ~PCSmoother() {};


    private:

        /// Type of code observation to be used.
        TypeID codeType;


        /// Type of phase observation to be used.
        TypeID phaseType;


        /// Type assigned to the resulting smoothed code.
        TypeID resultType;


        /// Maximum size of filter window, in samples.
        int maxWindowSize;


        /// Cycle slip flag #1. It should be present.
        TypeID csFlag1;


        /// Cycle slip flag #2. It should be present.
        TypeID csFlag2;


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
        virtual double getSmoothing(const SatID& sat, const double& code, const double& phase, const double& flag1, const double& flag2)
        {
            // In case we have a cycle slip either in L1 or L2
            if ( (flag1!=0.0) || (flag2!=0.0) )
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


        /// This method is out of reach in this class.
        virtual void setCodeType(const TypeID& dummy) {};

        /// This method is out of reach in this class.
        virtual void setPhaseType(const TypeID& dummy) {};

        /// This method is out of reach in this class.
        virtual void setCSFlag(const TypeID& dummy) {};

        /// This method is out of reach in this class.
        virtual TypeID getCSFlag() const { return TypeID::Unknown; };



        /// Initial index assigned to this class.
        static int classIndex;

        /// Index belonging to this object.
        int index;

        /// Sets the index and increment classIndex.
        void setIndex(void) { (*this).index = classIndex++; }; 



   }; // end class PCSmoother


   //@}
   
}

#endif
