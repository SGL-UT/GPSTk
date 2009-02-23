#pragma ident "$Id$"

/**
 * @file OneFreqCSDetector.hpp
 * This is a class to detect cycle slips using observables in just 
 * one frequency.
 */

#ifndef ONEFREQCSDETECTOR_HPP
#define ONEFREQCSDETECTOR_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//
//============================================================================



#include <deque>
#include "ProcessingClass.hpp"



namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{


      /** This is a class to detect cycle slips using observables in just one
       *  frequency.
       *
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
       *   while(rin >> gRin)
       *   {
       *      gRin >> markCSC1;
       *   }
       * @endcode
       *
       * The "OneFreqCSDetector" object will visit every satellite in the GNSS
       * data structure that is "gRin" and will decide if a cycle slip has
       * happened in the given observable.
       *
       * By default, the algorithm will use C1 and L1 observables, and the
       * LLI1 index. The result (a 1 if a cycle slip is found, 0 otherwise)
       * will be stored in the data structure as the CSL1 index.
       *
       * Note that these data types may be changed using the appropriate
       * methods. For example:
       *
       * @code
       *    markCSC1.setCodeType(TypeID::P2);
       *    markCSC1.setPhaseType(TypeID::L2);
       *    markCSC1.setLLIType(TypeID::LLI2);
       *    markCSC1.setResultType(TypeID::CSI2);
       * @endcode
       *
       * This algorithm will compute the bias between code and phase, and will
       * compare it with a mean bias that is computed on the fly. If the
       * current bias exceeds a given threshold, then a cycle slip is declared.
       *
       * The algorithm will also use the corresponding LLI index (and the RINEX
       * epoch flag, if present) to guide its decision.
       *
       * The threshold, as well as the filter window size and the maximum
       * allowed time interval between two successive measures, may be tuned
       * with their corresponding methods. For instance:
       *
       * @code
       *    markCSC1.setMaxNumSigmas(3.5);
       *    markCSC1.setMaxWindowSize(20);
       * @endcode
       *
       * Please be aware that the window size should not be too big, because
       * other factors (such as ionospheric drift) may show up in the bias,
       * affecting the algorithm. When using 1 Hz data sampling, a window size
       * between 60 and 100 samples will be fine.
       *
       * When used with the ">>" operator, this class returns the same incoming
       * data structure with the cycle slip index inserted along their
       * corresponding satellites.
       *
       * Be warned that if a given satellite does not have the  observations
       * required, it will be summarily deleted from the data structure.
       *
       * @sa LICSDetector.hpp, LICSDetector2.hpp and MWCSDetector.hpp for other
       * cycle slip detectors.
       *
       * \warning Cycle slip detectors are objets that store their internal
       * state, so you MUST NOT use the SAME object to process DIFFERENT data
       * streams.
       *
       */
   class OneFreqCSDetector : public ProcessingClass
   {
      public:

         /** Default constructor, setting default parameters and C1 and L1
          *  observables.
          */
      OneFreqCSDetector()
         : codeType(TypeID::C1), phaseType(TypeID::L1), lliType(TypeID::LLI1),
           resultType(TypeID::CSL1), deltaTMax(31.0), maxWindowSize(60),
           maxNumSigmas(4.5), defaultBiasSigma(4.0)
      { setIndex(); };


         /** Common constructor
          *
          * @param codeT         Type of code to be used.
          * @param dtMax         Maximum interval of time allowed between two
          *                      successive epochs.
          * @param mwSize        Maximum  size of filter window, in samples.
          * @param mnSigmas      Maximum deviation allowed before declaring
          *                      cycle slip (in number of sigmas).
          * @param dbSigma       Default value assigned to sigma when filter
          *                      starts, in meters.
          */
      OneFreqCSDetector( const TypeID& codeT,
                         const double& dtMax = 31.0,
                         const int& mwSize = 60,
                         const double& mnSigmas = 4.5,
                         const double& dbSigma = 4.0 );


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param epoch     Time of observations.
          * @param gData     Data object holding the data.
          * @param epochflag Epoch flag.
          */
      virtual satTypeValueMap& Process( const DayTime& epoch,
                                        satTypeValueMap& gData,
                                        const short& epochflag = 0 )
         throw(ProcessingException);


         /** Method to set the default code type to be used.
          *
          * @param codeT     TypeID of code to be used
          */
      virtual OneFreqCSDetector& setCodeType(const TypeID& codeT)
      { codeType = codeT; return (*this); };


         /// Method to get the default code type being used.
      virtual TypeID getCodeType() const
      { return codeType; };


         /** Method to set the default phase type to be used.
          *
          * @param phaseT    TypeID of phase to be used
          */
      virtual OneFreqCSDetector& setPhaseType(const TypeID& phaseT)
      { phaseType = phaseT; return (*this); };


         /// Method to get the default phase type being used.
      virtual TypeID getPhaseType() const
      { return phaseType; };


         /** Method to set the default LLI to be used.
          *
          * @param lliT    LLI to be used
          */
      virtual OneFreqCSDetector& setLLIType(const TypeID& lliT)
      { lliType = lliT; return (*this); };


         /// Method to get the default LLI being used.
      virtual TypeID getLLIType() const
      { return lliType; };


         /** Method to set the default return type to be used.
          *
          * @param returnT    TypeID to be returned
          */
      virtual OneFreqCSDetector& setResultType(const TypeID& resultT)
      { resultType = resultT; return (*this); };


         /// Method to get the default return type being used.
      virtual TypeID getResultType() const
      { return resultType; };


         /** Method to set the maximum interval of time allowed between two
          *  successive epochs.
          *
          * @param maxDelta      Maximum interval of time, in seconds
          */
      virtual OneFreqCSDetector& setDeltaTMax(const double& maxDelta)
      { deltaTMax = maxDelta; return (*this); };


         /// Method to get the maximum interval of time allowed between two
         /// successive epochs.
      virtual double getDeltaTMax() const
      { return deltaTMax; };


         /** Method to set the maximum size of filter window, in samples.
          *
          * @param maxSize       Maximum size of filter window, in samples.
          */
      virtual OneFreqCSDetector& setMaxWindowSize(const int& maxSize);


         /// Method to get the maximum size of filter window, in samples.
      virtual int getMaxWindowSize() const
      { return maxWindowSize; };


         /** Method to set the maximum deviation allowed before declaring cycle
          *  slip (in number of sigmas).
          *
          * @param maxNSigmas       Maximum deviation allowed before declaring
          *                         cycle slip (in number of sigmas).
          */
      virtual OneFreqCSDetector& setMaxNumSigmas(const double& maxNSigmas)
      { maxNumSigmas = maxNSigmas; return (*this); };


         /// Method to get the maximum deviation allowed before declaring cycle
         /// slip (in number of sigmas).
      virtual double getMaxNumSigmas() const
      { return maxNumSigmas; };


         /** Method to set the default value assigned to sigma when filter
          *  starts.
          *
          * @param defSigma      Default value assigned to sigma when
          *                      filter starts, in meters.
          */
      virtual OneFreqCSDetector& setDefaultBiasSigma(const double& defSigma)
      { defaultBiasSigma = defSigma; return (*this); };


         /// Method to get the default value assigned to sigma when
         /// filter starts, in meters.
      virtual double getDefaultBiasSigma() const
      { return defaultBiasSigma; };


         /** Returns a gnnsSatTypeValue object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException)
      { Process(gData.header.epoch, gData.body); return gData; };


         /** Returns a gnnsRinex object, adding the new data generated when
          *  calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException);


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


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


         /// Maximum deviation allowed before declaring cycle slip,
         /// in number of sigmas.
      double maxNumSigmas;


         /// Default value assigned to sigma when filter starts.
      double defaultBiasSigma;


        /// A structure used to store filter data for a SV.
      struct filterData
      {
            // Default constructor initializing the data in the structure
         filterData() : previousEpoch(DayTime::BEGINNING_OF_TIME),
                        windowSize(0), meanBias(0.0), variance(0.0)
         {};

         DayTime previousEpoch;  ///< The previous epoch time stamp.
         int windowSize;         ///< The filter window size.
         double meanBias;        ///< Accumulated mean bias
         double variance;        ///< Accumulated variance of bias.

         std::deque<double> biasBuffer;   ///< Values previous biases.
         std::deque<double> dif2Buffer;   ///< Values of previous differences^2.
      };


         /// Map holding the information regarding every satellite
      std::map<SatID, filterData> OneFreqData;


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param epoch     Time of observations.
          * @param sat       SatID.
          * @param tvMap     Data structure of TypeID and values.
          * @param epochflag Epoch flag.
          * @param code      Current code observation value.
          * @param phase     Current phase observation value.
          */
      virtual double getDetection( const DayTime& epoch,
                                   const SatID& sat,
                                   typeValueMap& tvMap,
                                   const short& epochflag,
                                   const double& code,
                                   const double& phase );


         /// Initial index assigned to this class.
      static int classIndex;


         /// Index belonging to this object.
      int index;


         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // End of class 'OneFreqCSDetector'

      //@}

}
#endif   // ONEFREQCSDETECTOR_HPP
