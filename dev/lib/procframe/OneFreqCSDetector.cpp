#pragma ident "$Id$"

/**
 * @file OneFreqCSDetector.cpp
 * This is a class to detect cycle slips using observables in just
 * one frequency.
 */

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


#include "OneFreqCSDetector.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int OneFreqCSDetector::classIndex = 3000000;


      // Returns an index identifying this object.
   int OneFreqCSDetector::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string OneFreqCSDetector::getClassName() const
   { return "OneFreqCSDetector"; }


      /* Common constructor
       *
       * @param codeT         Type of code to be used.
       * @param dtMax         Maximum interval of time allowed between two
       *                      successive epochs.
       * @param mwSize        Maximum  size of filter window, in samples.
       * @param mnSigmas      Maximum deviation allowed before declaring
       *                      cycle slip (in number of sigmas).
       * @param dbSigma       Default value assigned to sigma when filter
       *                      starts.
       */
   OneFreqCSDetector::OneFreqCSDetector( const TypeID& codeT,
                                         const double& dtMax,
                                         const int& mwSize,
                                         const double& mnSigmas,
                                         const double& dbSigma )
      : codeType(codeT), deltaTMax(dtMax), maxNumSigmas(mnSigmas),
        defaultBiasSigma(dbSigma)
   {

         // Don't allow window sizes less than 1
      if (mwSize >= 1)
      {
         maxWindowSize = mwSize;
      }
      else
      {
         maxWindowSize = 60;
      }

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

      setIndex();

   }  // End of constructor 'OneFreqCSDetector::OneFreqCSDetector()'



      /* Returns a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param epoch     Time of observations.
       * @param gData     Data object holding the data.
       * @param epochflag Epoch flag.
       */
   satTypeValueMap& OneFreqCSDetector::Process( const DayTime& epoch,
                                                satTypeValueMap& gData,
                                                const short& epochflag )
      throw(ProcessingException)
   {

      try
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
                  // If some value is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*it).first );
               continue;
            }

               // If everything is OK, then get the new value inside
               // the structure.
               // This way of doing it allows concatenation of several
               // different cycle slip detectors
            (*it).second[resultType] += getDetection( epoch,
                                                      (*it).first,
                                                      (*it).second,
                                                      epochflag,
                                                      value1,
                                                      value2 );

            if ( (*it).second[resultType] > 1.0 )
            {
               (*it).second[resultType] = 1.0;
            }

         }

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::int2x( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'OneFreqCSDetector::Process()'



      /* Method to set the maximum size of filter window, in samples.
       *
       * @param maxSize       Maximum size of filter window, in samples.
       */
   OneFreqCSDetector& OneFreqCSDetector::setMaxWindowSize(const int& maxSize)
   {

         // Don't allow window sizes less than 1
      if (maxSize >= 1)
      {
         maxWindowSize = maxSize;
      }
      else
      {
         maxWindowSize = 60;
      }

      return (*this);

   }  // End of method 'OneFreqCSDetector::setMaxWindowSize()'


      /* Returns a gnnsRinex object, adding the new data generated when
       *  calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& OneFreqCSDetector::Process(gnssRinex& gData)
      throw(ProcessingException)
   {

      try
      {

         Process(gData.header.epoch, gData.body, gData.header.epochFlag);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::int2x( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }


      /* Returns a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param epoch     Time of observations.
       * @param sat       SatID.
       * @param tvMap     Data structure of TypeID and values.
       * @param epochflag Epoch flag.
       * @param code      Current code observation value.
       * @param phase     Current phase observation value.
       */
   double OneFreqCSDetector::getDetection( const DayTime& epoch,
                                           const SatID& sat,
                                           typeValueMap& tvMap,
                                           const short& epochflag,
                                           const double& code,
                                           const double& phase )
   {

      bool reportCS(false);
      double deltaT(0.0);        // Difference between current and former
                                 // epochs, in seconds
      double bias(0.0);          // Code-phase bias
      double dif2(0.0);          // Difference between biases, squared.
      double thr2(0.0);          // Threshold in sigmas, squared.
      double deltaBias(0.0);     // Difference between biases
      double tempLLI(0.0);

         // Get the difference between current epoch and former epoch,
         // in seconds
      deltaT = ( epoch.MJDdate() - OneFreqData[sat].previousEpoch.MJDdate() )
               * DayTime::SEC_DAY;

         // Store current epoch as former epoch
      OneFreqData[sat].previousEpoch = epoch;

      bias = code - phase;       // Current value of code-phase bias

         // Increment size of window and check limit
      ++OneFreqData[sat].windowSize;
      if (OneFreqData[sat].windowSize > maxWindowSize)
      {
         OneFreqData[sat].windowSize = maxWindowSize;
      }

         // Check if receiver already declared cycle slip or too much time
         // has elapsed
         // Note: If tvMap(lliType) doesn't exist, then 0 will be returned
         // and that test will pass
      if ( (tvMap(lliType)==1.0) ||
           (tvMap(lliType)==3.0) ||
           (tvMap(lliType)==5.0) ||
           (tvMap(lliType)==7.0) )
      {
         tempLLI = 1.0;
      }

      if ( (epochflag==1)        ||
           (epochflag==6)        ||
           (tempLLI==1.0)        ||
           (deltaT > deltaTMax) )
      {
         OneFreqData[sat].windowSize = 1;
      }

      if (OneFreqData[sat].windowSize > 1)
      {

         deltaBias = (bias - OneFreqData[sat].meanBias);

            // Square difference between biases
         dif2 = deltaBias*deltaBias;

            // Compute threshold^2
         thr2 = OneFreqData[sat].meanSigma2 * maxNumSigmas * maxNumSigmas;

            // If difference in biases is bigger or equal to threshold,
            // then declare cycle slip
         if (dif2 >= thr2)
         {
            OneFreqData[sat].windowSize = 1;
         }
         else
         {
               // Update mean bias
            OneFreqData[sat].meanBias = OneFreqData[sat].meanBias +
                  deltaBias/(static_cast<double>(OneFreqData[sat].windowSize));

               // Update mean variance
            OneFreqData[sat].meanSigma2 = OneFreqData[sat].meanSigma2 +
                           ( (dif2-OneFreqData[sat].meanSigma2) ) /
                           (static_cast<double>(OneFreqData[sat].windowSize));
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

      if (reportCS)
      {
         return 1.0;
      }
      else
      {
         return 0.0;
      }

   }  // End of method 'OneFreqCSDetector::getDetection()'


}  // End of namespace gpstk
