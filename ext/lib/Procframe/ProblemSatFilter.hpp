//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010, 2011
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file ProblemSatFilter.hpp
 * This class filters out satellites that are in maneuvers or bad data.
 */

#ifndef GPSTK_PROBLEMSATFILTER_HPP
#define GPSTK_PROBLEMSATFILTER_HPP

#include <iostream>
#include <string>
#include "ProcessingClass.hpp"


namespace gpstk
{

      /// @ingroup GPSsolutions
      //@{


      /** This class filters out satellites that are in the Satellite Problem 
       *  File(*.CRX) of Bernese GPS Software 5.0. These CRX files can be
       *  downloaded from 'ftp.unibe.ch/aiub/BSWUSER50/GEN'.
       *  
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *      // Input observation file stream
       *   RinexObsStream rin("ebre0300.02o");
       *
       *   ProblemSatFilter satFilter;
       *   satFilter.loadSatelliteProblemFile("SAt_2010.CRX");
       *
       *   gnssRinex gRin;  // GNSS data structure for fixed station data
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> satFilter;
       *   }
       * @endcode
       *
       *
       */
   class ProblemSatFilter : public ProcessingClass
   {
   public:

         /// Default constructor.
      ProblemSatFilter() 
      { };


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param epoch     Time of observations.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& epoch,
                                        satTypeValueMap& gData )
         throw(ProcessingException);


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


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Load Satellite Problem File(*.CRX) download from
         /// 'ftp.unibe.ch/aiub/BSWUSER50/GEN'
      int loadSatelliteProblemFile(const std::string& crxFile);


         /// Clear all data
      void clear()
      { satDataMap.clear(); }

      
         /// Destructor
      virtual ~ProblemSatFilter() {};

   protected:
      bool isBadSat(const CommonTime& time,const SatID& sat);

   protected:

      struct SatData
      {
         int spiltFlag;
         int problemFlag;
         int actionFlag;
         CommonTime startEpoch;
         CommonTime endEpoch;

         SatData():spiltFlag(0),problemFlag(0),actionFlag(0),
                   startEpoch(CommonTime::BEGINNING_OF_TIME),
                   endEpoch(CommonTime::BEGINNING_OF_TIME)
         {}
      };

      typedef std::list<SatData> SatDataList;
      typedef std::map<SatID, SatDataList > SatDataMap;
         
         /// Object to holding all the data
      SatDataMap  satDataMap;


   }; // End of class 'ProblemSatFilter'

      //@}
}

#endif   // GPSTK_PROBLEMSATFILTER_HPP
