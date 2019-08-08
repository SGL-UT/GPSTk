//==============================================================================
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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
  *  @file OrbElemRinex.hpp
  *  Contains the "engineering units" contents of a set of subframe 1/2/3 GPS
  *  navigation message data as created from a record in a Rinex navigation
  *  message file.
  *  This class inherits from the OrbElem class and provides both the capability
  *  to load an OrbElem from a set of Rinex data and storage for the Rinex-specific
  *  parts of the Rinex file.
  */ 

#ifndef GPSTK_ORBELEMRINEX_HPP
#define GPSTK_ORBELEMRINEX_HPP

#include <string>
#include <iostream>

#include "OrbElem.hpp"
#include "RinexNavData.hpp"
#include "Rinex3NavData.hpp"


namespace gpstk
{
   class OrbElemRinex : public OrbElem
   {
   public:
         /// Default constructor
      OrbElemRinex();

      OrbElemRinex( const Rinex3NavData& rinNav )
         throw( InvalidParameter );

      OrbElemRinex( const RinexNavData& rinNav )
	 throw( InvalidParameter); 

         /// Destructor
      virtual ~OrbElemRinex() {}

        /// Clone method
      virtual OrbElemRinex* clone() const;

         /**  Load an existing object from a RinexNavData object.
           *  @throw InvalidParameter if the data are not consistent.
           */ 
      void loadData( const RinexNavData& rinNav )
	       throw( InvalidParameter); 

         /** Load an existing object from a Rinex3NavData object. 
           * @throw InvalidParameter if the data are not consistent.
           */
      void loadData( const Rinex3NavData& rinNav )
         throw( InvalidParameter );

      virtual std::string getName() const
      {
         return "OrbElemRinex";
      }
 
      virtual std::string getNameLong() const
      {
         return "Rinex Navigation Message";
      }
      
         /// Returns the upper bound of the URA range
      double getAccuracy()  const
         throw( InvalidRequest );
 
        /* Should only be used by GPSOrbElemStore::rationalize()
         */
      void adjustBeginningValidity();

      virtual void dumpHeader(std::ostream& s = std::cout) const
         throw( InvalidRequest );

         /** Generate a formatted human-readable output of the entire contents of
          *  this object and send it to the designated output stream (default to cout).
          *  @throw Invalid Parameter if the object has been instantiated, but not loaded.
          */
      void dump(std::ostream& s = std::cout) const
         throw( InvalidRequest );  
         /** Generate a formatted human-readable one-line output that summarizes
          *  the critical times associated with this object and send it to the
          *  designated output stream (default to cout).
          *  @throw Invalid Parameter if the object has been instantiated, but not loaded.
          */   
      void dumpTerse(std::ostream& s = std::cout) const
         throw( InvalidRequest );

   private:
      void determineTimes();
      void determineTimesGPS();
      void determineTimesGalileo();
      void determineTimesDefault();

   public:
              /// Ephemeris overhead information
         //@{
      
      CommonTime transmitTime; /** Estimated beginning time of this sample */

      short  codeflags;     /**< L2 codes */
      double accuracyValue; /**< User Range Accuracy (meters) */ 
      short  health;        /**< SV health */
      short  L2Pdata;       /**< L2 P data flag */
      short  IODC;          /**< Index of data-clock  */
      short  fitDuration;   /**< Fit duration (hours) */
      double Tgd;           /**< L1 and L2 correction term */
      long   HOWtime;        /**< Handover Word time */

         // Static and public for test support pruposes.  Should only be used with OrbElemCNAV and OrbElemCNAV2
         // in production.
         //
         // Determining the earliest transmit time is deterministic for non-upload cutovers 
         // and needs to be done correctly in order to replicated user experience in post-processing, 
         // even if the actual data were collected later than earliest transmission. However, in the
         // case of upload cutovers, we have to depend on continuous collection to properly detect
         // upload cutover times. 
      static CommonTime computeBeginValid(const CommonTime& xmit,
                                          const CommonTime& ctToe ); 
      static CommonTime computeEndValid(const CommonTime& ctToe,
                                        const int fitHours );
      static bool  isNominalToe(const CommonTime& ctToe);

      static long TWO_HOURS;
      static long SIXTEEN_SECONDS;

   private:
         // The following two members are used for communicatiaon within
         // OrbElemRinex.
      double Toc3;            // As read from RINEX
      double Toe3;
      short fullXmitWeekNum; // As read from RINEX


         //@}

   }; // end class OrbElemRinex

   //@}

   std::ostream& operator<<(std::ostream& s, 
                                     const OrbElemRinex& eph);

} // end namespace 

#endif // GPSTK_ORBELEMRINEX_HPP
