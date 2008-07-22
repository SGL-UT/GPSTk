#pragma ident "$Id$"

/**
 * @file MWCSDetector.hpp
 * This is a class to detect cycle slips using the Melbourne-Wubbena
 * combination.
 */

#ifndef MWCSDETECTOR_HPP
#define MWCSDETECTOR_HPP

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



#include "ProcessingClass.hpp"
#include <list>


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{


      /** This is a class to detect cycle slips using MW observables.
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
       *   ComputeMelbourneWubbena getMW;
       *   MWCSDetector markCSMW;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> getMW >> markCSMW;
       *   }
       * @endcode
       *
       * The "MWCSDetector" object will visit every satellite in the GNSS data
       * structure that is "gRin" and will decide if a cycle slip has happened
       * in the given observable.
       *
       * The algorithm will use MW observables, and the LLI1 and LLI2 indexes.
       * The result (a 1 if a cycle slip is found, 0 otherwise) will be stored
       * in the data structure both as the CSL1 and CSL2 indexes.
       *
       * In taking the decision, this algorithm will use criteria as the
       * maximum interval of time between two successive epochs and the
       * maximum number of Melbourne-Wubbena wavelenghts allowed above or
       * below the MW combination average for that arc.
       *
       * The default values are usually fine, but you may change them with the
       * appropriate methods. This is of special importance for the maximum
       * interval time, that should be adjusted for your sampling rate. It is
       * 61 seconds by default, which is appropriate for 30 seconds per sample
       * RINEX observation files.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the cycle slip indexes inserted along
       * their corresponding satellites. Be warned that if a given satellite
       * does not have the observations required, it will be summarily deleted
       * from the data structure.
       *
       * You should be aware that the Melbourne-Wubbena combination is based
       * on a mix of code and phase observations, so it is very noisy.
       * Therefore, it has a tendency to yield a high number of false
       * positives if you are not careful with its parameters. Because of
       * this, the default parameters are very conservative, i.e., the
       * detector is NOT very sensitive by default.
       *
       * Best results are achieved when using this detector as a "backup"
       * detector for detectors based in LI combination, like this:
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
       *   while(rin >> gRin)
       *   {
       *      gRin >> getLI >> getMW >> markCSLI >> markCSMW;
       *   }
       * @endcode
       *
       * @sa LICSDetector.hpp for more information.
       *
       * \warning Cycle slip detectors are objets that store their internal
       * state, so you MUST NOT use the SAME object to process DIFFERENT data
       * streams.
       *
       */
   class MWCSDetector : public ProcessingClass
   {
   public:

         /// Default constructor, setting default parameters.
      MWCSDetector() : obsType(TypeID::MWubbena), lliType1(TypeID::LLI1),
                       lliType2(TypeID::LLI2), resultType1(TypeID::CSL1),
                       resultType2(TypeID::CSL2), deltaTMax(61.0),
                       maxNumLambdas(10.0), useLLI(true)
      { setIndex(); };


         /** Common constructor
          *
          * @param mLambdas      Maximum deviation allowed before declaring
          *                      cycle slip (in number of Melbourne-Wubbena
          *                      wavelenghts).
          * @param dtMax         Maximum interval of time allowed between two
          *                      successive epochs, in seconds.
          */
      MWCSDetector( const double& mLambdas,
                    const double& dtMax = 61.0,
                    const bool& use = true );


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


         /** Method to set the maximum interval of time allowed between two
          *  successive epochs.
          *
          * @param maxDelta      Maximum interval of time, in seconds
          */
      virtual MWCSDetector& setDeltaTMax(const double& maxDelta);


         /** Method to get the maximum interval of time allowed between two
          *  successive epochs, in seconds.
          */
      virtual double getDeltaTMax() const
      { return deltaTMax; };


         /** Method to set the maximum deviation allowed before declaring
          *  cycle slip (in number of Melbourne-Wubbena wavelenghts).
          *
          * @param mLambdas     Maximum deviation allowed before declaring
          *                     cycle slip (in number of Melbourne-Wubbena
          *                     wavelenghts).
          */
      virtual MWCSDetector& setMaxNumLambdas(const double& mLambdas);


         /** Method to get the maximum deviation allowed before declaring
          *  cycle slip (in number of Melbourne-Wubbena wavelenghts).
          */
      virtual double getMaxNumLambdas() const
      { return maxNumLambdas; };


         /** Method to set whether the LLI indexes will be used as
          *  an aid or not.
          *
          * @param use   Boolean value enabling/disabling LLI check.
          */
      virtual MWCSDetector& setUseLLI(const bool& use)
      { useLLI = use; return (*this); };


         /// Method to know if the LLI check is enabled or disabled.
      virtual bool getUseLLI() const
      { return useLLI; };


         /** Returns a gnnsSatTypeValue object, adding the new data generated
          *  when calling this object.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException)
      { (*this).Process(gData.header.epoch, gData.body); return gData; };


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
      virtual ~MWCSDetector() {};


   private:


         /// Type of observation.
      TypeID obsType;


         /// Type of LMW1 record.
      TypeID lliType1;


         /// Type of LMW2 record.
      TypeID lliType2;


         /// Type of result #1.
      TypeID resultType1;


         /// Type of result #2.
      TypeID resultType2;


         /** Maximum interval of time allowed between two successive
          *  epochs, in seconds.
          */
      double deltaTMax;


         /** Maximum deviation allowed before declaring cycle slip,
          *  in number of Melbourne-Wubbena wavelenghts.
          */
      double maxNumLambdas;


         /// Whether use or ignore the LLI indexes as an aid. 
      bool useLLI;


         /// A structure used to store filter data for a SV.
      struct filterData
      {
            // Default constructor initializing the data in the structure
         filterData() : formerEpoch(DayTime::BEGINNING_OF_TIME),
                        windowSize(0), meanMW(0.0) {};

         DayTime formerEpoch;    ///< The previous epoch time stamp.
         int windowSize;         ///< Size of current window, in samples.
         double meanMW;          ///< Accumulated mean value of combination.
      };


         /// Map holding the information regarding every satellite
      std::map<SatID, filterData> MWData;


         /** Method that implements the Melbourne-Wubbena cycle slip
          *  detection algorithm
          *
          * @param epoch     Time of observations.
          * @param sat       SatID.
          * @param tvMap     Data structure of TypeID and values.
          * @param epochflag Epoch flag.
          * @param mw        Current MW observation value.
          * @param lli1      LLI1 index.
          * @param lli2      LLI2 index.
          */
      virtual double getDetection( const DayTime& epoch,
                                   const SatID& sat,
                                   typeValueMap& tvMap,
                                   const short& epochflag,
                                   const double& mw,
                                   const double& lli1,
                                   const double& lli2 );


         /// Initial index assigned to this class.
      static int classIndex;


         /// Index belonging to this object.
      int index;


         /// Sets the index and increment classIndex.
      void setIndex(void)
      { (*this).index = classIndex++; };


   }; // End of class 'MWCSDetector'

      //@}

}
#endif   // MWCSDETECTOR_HPP
