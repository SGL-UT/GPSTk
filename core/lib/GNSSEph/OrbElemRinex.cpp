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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file OrbElemRinex.cpp
 * OrbElemRinex data encapsulated in engineering terms
 */
#include <iomanip>
#include <cmath>

#include "OrbElemRinex.hpp"
#include "StringUtils.hpp"
#include "GNSSconstants.hpp"
#include "GPS_URA.hpp"
#include "BDSWeekSecond.hpp"
#include "GALWeekSecond.hpp"
#include "GPSWeekSecond.hpp"
#include "IRNWeekSecond.hpp"
#include "QZSWeekSecond.hpp"
#include "TimeString.hpp"

namespace gpstk
{
   using namespace std;

   long OrbElemRinex::TWO_HOURS   = 7200;
   long OrbElemRinex::SIXTEEN_SECONDS  =   16;

   //----------------------------------------------------------------
   OrbElemRinex::OrbElemRinex()
      : OrbElem(),
        codeflags(0), accuracyValue(0.0), health(0),
        L2Pdata(0), IODC(0), fitDuration(0), Tgd(0.0)
   {}

   //----------------------------------------------------------------
   OrbElemRinex::OrbElemRinex( const RinexNavData& rinNav )
   {
      loadData( rinNav );
   }

   //----------------------------------------------------------------
   OrbElemRinex::OrbElemRinex( const Rinex3NavData& rinNav )
   {
      loadData( rinNav );
   }

     /// Clone method
   //----------------------------------------------------------------
   OrbElemRinex* OrbElemRinex::clone() const
   {
      return new OrbElemRinex (*this);
   }

   //----------------------------------------------------------------
   void OrbElemRinex::loadData( const RinexNavData& rinNav )
   {
      // Fill in the variables unique to OrbElemFIC9
      codeflags        = rinNav.codeflgs;
      accuracyValue    = rinNav.accuracy;
      setHealth(rinNav.health);
      IODC             = rinNav.IODC;
      L2Pdata          = rinNav.L2Pdata;
      Tgd              = rinNav.Tgd;

      HOWtime        = rinNav.getHOWWS().sow;
      fitDuration    = rinNav.fitint;

         // really the HOW week
      short fullXmitWeekNum    = rinNav.getHOWWS().week;

         // Fill in the variables in the OrbElem parent
	 // - - - First the simple copies - - -
         // OrbElem only stores fully qualified times, but this was
         // hacked in as part of updating RinexNavData to handle times
         // according to the spec.
         /// @todo update this code to use the newer accessors of RinexNavData
      double Toc     = rinNav.getTocWS().sow;
                                         // see notes below.
      af0            = rinNav.af0;
      af1            = rinNav.af1;
      af2            = rinNav.af2;

      Cuc            = rinNav.Cuc;
      Cus            = rinNav.Cus;
      Crc            = rinNav.Crc;
      Crs            = rinNav.Crs;
      Cic            = rinNav.Cic;
      Cis            = rinNav.Cis;


      double Toe     = rinNav.Toe; 
      M0             = rinNav.M0;       // OrbElem only stores fully qualified times
      dn             = rinNav.dn;       // see notes below.
      ecc            = rinNav.ecc;
      double AHalf   = rinNav.Ahalf;
      OMEGA0         = rinNav.OMEGA0;
      i0             = rinNav.i0;
      w              = rinNav.w;
      OMEGAdot       = rinNav.OMEGAdot;
      idot           = rinNav.idot;

      // - - - Now work on the things that need to be calculated - - -

    // The system is assumed (legacy navigation message is from GPS)
      satID.id     = static_cast<short>( rinNav.PRNID );
      satID.system = SatelliteSystem::GPS;
      if (satID.id>=MIN_PRN_QZS && 
          satID.id<=MAX_PRN_QZS) 
         satID.system = SatelliteSystem::QZSS;  

         // The observation ID has a type of navigation, but the
         // carrier and code types are undefined.  They could be
         // L1/L2 C/A, P, Y,.....
      obsID.type = ObservationType::NavMsg;
      obsID.band = CarrierBand::Undefined;
      obsID.code = TrackingCode::Undefined;

	 // Beginning of Validity
         // New concept.  Admit the following.
	 //  (a.) The collection system may not capture the data at earliest transmit.
	 //  (b.) The collection system may not capture the three SFs consecutively.
	 // Consider a couple of IS-GPS-200 promises,
	 //  (c.) By definition, beginning of validity == beginning of transmission.
	 //  (d.) Except for uploads, cutovers will only happen on hour boundaries
	 //  (e.) Cutovers can be detected by non-even Toc.
	 //  (f.) Even uploads will cutover on a frame (30s) boundary.
         // Therefore,
	 //   1.) If Toc is NOT even two hour interval, pick lowest HOW time,
	 //   round back to even 30s.  That's the earliest Xmit time we can prove.
	 //   NOTE: For the case where this is the SECOND SF 1/2/3 after an upload,
	 //   this may yield a later time as such a SF 1/2/3 will be on a even
	 //   hour boundary.  Unfortunately, we have no way of knowing whether
	 //   this item is first or second after upload without additional information.
    //
    //   Item 2 moved to computeBeginValid() in Jan 2019 as GPS III
    //   changes are handled. Note that item 2 is really the beginValid
    //   time as opposed to the transmit time. 
	 //   2.) If Toc IS even two hour interval, pick time from SF 1,
	 //   round back to nearest EVEN two hour boundary.  This assumes collection
	 //   SOMETIME in first hour of transmission.  Could be more
	 //   complete by looking at fit interval and IODC to more accurately
	 //   determine earliest transmission time.
	 //
	 //   3.) SPECIAL CASE to address oddity in IGS brdc aggregate files. 
	 //   At the beginning of day, it appears the some stations report 
	 //   the last set of the previous day with a "transmit time" of 0 SOD 
	 //   and a Toc of 0 SOD.  This is errant nonsense, but its in the data.
	 //   I suspect someone's "daily file writer" is dumping out the last 
	 //   SF 1/2/3 of the previous day with the earliest valid time for the
	 //   current day. 
	 //   SO - If the "transmit time" claims 0 SOD with the Toc of 0 SOD, we'll
	 //   nudge the "tranmist time" back into the previous day. 
      long longToc = (long) Toc;

         // Case 3 check
      long adjHOWtime = HOWtime;
      if ((longToc % SEC_PER_DAY) == 0 &&
          (HOWtime % SEC_PER_DAY) == 0 &&
           longToc == HOWtime            )
      {
         adjHOWtime = HOWtime - 30;
         if (adjHOWtime<0)
         {
            adjHOWtime += FULLWEEK;  
            fullXmitWeekNum--;     
         }
      }
      
      // Determine Transmit Time
      // Transmit time is the actual time this
      // SF 1/2/3 sample was collected
      long Xmit = adjHOWtime - (adjHOWtime % 30);
      double XmitSOW = (double) Xmit;
      transmitTime = GPSWeekSecond( fullXmitWeekNum, (double)Xmit, TimeSystem::GPS );

         // Fully qualified Toe and Toc
	 // As broadcast, Toe and Toc are in GPS SOW and do not include
	 // the GPS week number.  OrbElem (rightly) insists on having a
	 // Toe and Toc in CommonTime objects which implies determining
	 // the week number.
      double timeDiff = Toe - XmitSOW;
      short epochWeek = fullXmitWeekNum;
      if (timeDiff < -HALFWEEK) epochWeek++;
      else if (timeDiff > HALFWEEK) epochWeek--;

      ctToc = GPSWeekSecond(epochWeek, Toc, TimeSystem::GPS);
      ctToe = GPSWeekSecond(epochWeek, Toe, TimeSystem::GPS);

      beginValid = computeBeginValid(satID, transmitTime, ctToe); 
      endValid = computeEndValid(ctToe,fitDuration); 

	 // Semi-major axis and time-rate-of-change of semi-major axis
	 //    Note: Legacy navigation message (SF 1/2/3) used SQRT(A).
	 //    The CNAV and CNAV-2 formats use deltaA and Adot.  As a
	 //    result, OrbElem uses A and Adot and SQRT(A) and deltaA
	 //    are converted to A at runtime.
      A = AHalf * AHalf;
      Adot = 0.0;
         // Legacy nav doesn't have Rate of Change to Correction to mean motion,
	 // so set it to zero.
      dndot = 0.0;

         // After all this is done, declare that data has been loaded
	 // into this object (so it may be used).
      dataLoadedFlag = true;
   }

   //----------------------------------------------------------------
   void OrbElemRinex::loadData( const Rinex3NavData& rinNav )
   {
      // Fill in the variables unique to OrbElemFIC9
      codeflags        = rinNav.codeflgs;
      accuracyValue    = rinNav.accuracy;
      setHealth(rinNav.health);
      IODC             = rinNav.IODC;
      L2Pdata          = rinNav.L2Pdata;
      Tgd              = rinNav.Tgd;

      HOWtime          = rinNav.xmitTime;
      fitDuration      = rinNav.fitint;

      fullXmitWeekNum  = rinNav.weeknum;

         // Fill in the variables in the OrbElem parent
	 // - - - First the simple copies - - -
      Toc3           = rinNav.Toc;       // OrbElem only stores fully qualified times
                                         // see notes below.
      af0            = rinNav.af0;
      af1            = rinNav.af1;
      af2            = rinNav.af2;

      Cuc            = rinNav.Cuc;
      Cus            = rinNav.Cus;
      Crc            = rinNav.Crc;
      Crs            = rinNav.Crs;
      Cic            = rinNav.Cic;
      Cis            = rinNav.Cis;

      Toe3           = rinNav.Toe; 
      M0             = rinNav.M0;       // OrbElem only stores fully qualified times
      dn             = rinNav.dn;       // see notes below.
      ecc            = rinNav.ecc;
      double AHalf   = rinNav.Ahalf;
      OMEGA0         = rinNav.OMEGA0;
      i0             = rinNav.i0;
      w              = rinNav.w;
      OMEGAdot       = rinNav.OMEGAdot;
      idot           = rinNav.idot;

      // - - - Now work on the things that need to be calculated - - -

      satID.id     = rinNav.sat.id;
      satID.system = rinNav.sat.system; 

         // Galileo-specific override
         // This points out that the existing OrbElemRinex is wholly inadequate to the 
         // task of representing all the special cases in Rinex V3.
      if (satID.system==SatelliteSystem::Galileo)
      {
         IODC = rinNav.IODnav;
      }

         // The observation ID has a type of navigation, but the
         // carrier and code types are undefined.  They could be
         // L1/L2 C/A, P, Y,.....
      obsID.type = ObservationType::NavMsg;
      obsID.band = CarrierBand::Undefined;
      obsID.code = TrackingCode::Undefined;

         // The rules for deriving Toe, Toc, begin valid, begin transmit,
         // and end valid are similar, but system-specific. 
         // Therefore, we'll encapsulate all this into a set of methods.
      determineTimes();

	      // Semi-major axis and time-rate-of-change of semi-major axis
	      //    Note: Legacy navigation message (SF 1/2/3) used SQRT(A).
	      //    The CNAV and CNAV-2 formats use deltaA and Adot.  As a
	      //    result, OrbElem uses A and Adot and SQRT(A) and deltaA
	      //    are converted to A at runtime.
      A = AHalf * AHalf;
      Adot = 0.0;
         // Legacy nav doesn't have Rate of Change to Correction to mean motion,
	      // so set it to zero.
      dndot = 0.0;

         // After all this is done, declare that data has been loaded
	      // into this object (so it may be used).
      dataLoadedFlag = true;
   }

   //----------------------------------------------------------------
   double OrbElemRinex::getAccuracy()  const
   {
      if (!dataLoaded())
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return ( accuracyValue );
   }


   //----------------------------------------------------------------
   // The rules for deriving Toe, Toc, begin valid, begin transmit,
   // and end valid are similar, but system-specific. 
   void OrbElemRinex::determineTimes()
   {
      switch (satID.system)
      {
         case SatelliteSystem::GPS:     { determineTimesGPS();     break; }
         case SatelliteSystem::Galileo: { determineTimesGalileo(); break; }
         default:                   { determineTimesDefault(); }
      }
      return;
   }

   //----------------------------------------------------------------
   // Beginning of Validity
   // New concept.  Admit the following.
   //  (a.) The collection system may not capture the data at earliest transmit.
   //  (b.) The collection system may not capture the three SFs consecutively.
   // Consider a couple of IS-GPS-200 promises,
   //  (c.) By definition, beginning of validity == beginning of transmission.
   //  (d.) Except for uploads, cutovers will only happen on hour boundaries
   //  (e.) Cutovers can be detected by non-even Toc.
   //  (f.) Even uploads will cutover on a frame (30s) boundary.
   // Therefore,
   //   1.) If Toc is NOT even two hour interval, pick lowest HOW time,
   //   round back to even 30s.  That's the earliest Xmit time we can prove.
   //   NOTE: For the case where this is the SECOND SF 1/2/3 after an upload,
   //   this may yield a later time as such a SF 1/2/3 will be on a even
   //   hour boundary.  Unfortunately, we have no way of knowing whether
   //   this item is first or second after upload without additional information.
   //
   //   Item 2 moved to computeBeginValid() in Jan 2019 as GPS III
   //   changes are handled. Note that item 2 is really the beginValid
   //   time as opposed to the transmit time. 
   //   2.) If Toc IS even two hour interval, pick time from SF 1,
   //   round back to nearest EVEN two hour boundary.  This assumes collection
   //   SOMETIME in first hour of transmission.  Could be more
   //   complete by looking at fit interval and IODC to more accurately
   //   determine earliest transmission time.
   //
   //   3.) SPECIAL CASE to address oddity in IGS brdc aggregate files. 
   //   At the beginning of day, it appears the some stations report 
   //   the last set of the previous day with a "transmit time" of 0 SOD 
   //   and a Toc of 0 SOD.  This is errant nonsense, but its in the data.
   //   I suspect someone's "daily file writer" is dumping out the last 
   //   SF 1/2/3 of the previous day with the earliest valid time for the
   //   current day. 
   //   SO - If the "transmit time" claims 0 SOD with the Toc of 0 SOD, we'll
   //   nudge the "transmit time" back into the previous day. 
   void OrbElemRinex::determineTimesGPS()
   {
      long longToc = (long) Toc3;

         // Case 3 check
      long adjHOWtime = HOWtime;
      if ((longToc % SEC_PER_DAY) == 0 &&
          (HOWtime % SEC_PER_DAY) == 0 &&
           longToc == HOWtime            )
      {
         adjHOWtime = HOWtime - 30;
         if (adjHOWtime<0)
         {
            adjHOWtime += FULLWEEK;
            fullXmitWeekNum--;
         }
      }

      // Determine Transmit Time
      // Transmit time is the actual time this
      // SF 1/2/3 sample was collected
      long Xmit = adjHOWtime - (adjHOWtime % 30);
      double XmitSOW = (double) Xmit;
      transmitTime = GPSWeekSecond( fullXmitWeekNum, (double)Xmit, TimeSystem::GPS );

         // Fully qualified Toe and Toc
         // As broadcast, Toe and Toc are in GPS SOW and do not include
         // the GPS week number.  OrbElem (rightly) insists on having a
         // Toe and Toc in CommonTime objects which implies determining
         // the week number.
      double timeDiff = Toe3 - XmitSOW;
      short epochWeek = fullXmitWeekNum;
      if (timeDiff < -HALFWEEK) epochWeek++;
      else if (timeDiff > HALFWEEK) epochWeek--;

      ctToc = GPSWeekSecond(epochWeek, Toc3, TimeSystem::GPS);
      ctToe = GPSWeekSecond(epochWeek, Toe3, TimeSystem::GPS);
      beginValid = computeBeginValid(satID, transmitTime, ctToe); 
      endValid = computeEndValid(ctToe,fitDuration);                 
   }

   //----------------------------------------------------------------
   // All we can say for Galileo is that the earliest transmit time
   // is equivalent to the HOWTime rounded back to the beginning 
   // of the subframe.  Unfortunately, in RINEX we've lost reference to
   // which band/code the data were collected from.   Therefore, we can only
   // go with the HOWtime.
   // However, we'll go ahead and define a specific method against the hope
   // that we will figure out some brilliant way around this later. 
   void OrbElemRinex::determineTimesGalileo()
   {
      fullXmitWeekNum -= 1024;    // RINEX 3 stores GPS Week numbers. 
                                  // Need to move to Galileo week numbers.

      long longToc = (long) Toc3;

         // Check for incorrectly tagged BOD data.
      long adjHOWtime = HOWtime;
      if ((longToc % SEC_PER_DAY) == 0 &&
          (HOWtime % SEC_PER_DAY) == 0 &&
           longToc == HOWtime            )
      {
         adjHOWtime = HOWtime - 30;
         if (adjHOWtime<0)
         {
            adjHOWtime += FULLWEEK;
            fullXmitWeekNum--;
         }
      }

      // Determine Transmit Time
      // Transmit time is the actual time this
      // SF 1/2/3 sample was collected
      double XmitSOW = adjHOWtime;
      beginValid   = GALWeekSecond(fullXmitWeekNum, XmitSOW,   TimeSystem::GAL);
      transmitTime = GALWeekSecond(fullXmitWeekNum, XmitSOW,   TimeSystem::GAL);

         // Fully qualified Toe and Toc
         // As broadcast, Toe and Toc are in GPS SOW and do not include
         // the GPS week number.  OrbElem (rightly) insists on having a
         // Toe and Toc in CommonTime objects which implies determining
         // the week number.
      double timeDiff = Toe3 - XmitSOW;
      short epochWeek = fullXmitWeekNum;
      if (timeDiff < -HALFWEEK) epochWeek++;
      else if (timeDiff > HALFWEEK) epochWeek--;
      ctToc        = GALWeekSecond(epochWeek,       Toc3,       TimeSystem::GAL);
      ctToe        = GALWeekSecond(epochWeek,       Toe3,       TimeSystem::GAL);

         // End of Validity.
         // Galileo doens't have a fit interval, for the SDD claims healthy
         // messages will never be valid for more than 4 hours from beginning of transmission.
      endValid =  beginValid  + 4 * 3600.0;
/*
      string tform1("%02m/%02d/%04Y %02H:%02M:%02S %P");
      cout << "Time Summary: " << endl;
      cout << "   beginValid: " << printTime(beginValid,tform1) << endl;
      cout << " transmitTime: " << printTime(transmitTime,tform1) << endl;
      cout << "          Toe: " << printTime(ctToe,tform1) << endl;
      cout << "          Toc: " << printTime(ctToc,tform1) << endl;
      cout << "     endValid: " << printTime(endValid,tform1) << endl;
 */
   }

   //----------------------------------------------------------------
   // For the default case, simply accept the HOWtime as the
   // beginning of effectivity.
   void OrbElemRinex::determineTimesDefault()
   {
      long longToc = (long) Toc3;

         // Check for incorrectly tagged BOD data.
      long adjHOWtime = HOWtime;
      if ((longToc % SEC_PER_DAY) == 0 &&
          (HOWtime % SEC_PER_DAY) == 0 &&
           longToc == HOWtime            )
      {
         adjHOWtime = HOWtime - 30;
         if (adjHOWtime<0)
         {
            adjHOWtime += FULLWEEK;
            fullXmitWeekNum--;
         }
      }

      // Determine Transmit Time
      // Transmit time is the actual time this
      // SF 1/2/3 sample was collected
      double XmitSOW = adjHOWtime;

         // Fully qualified Toe and Toc
         // As broadcast, Toe and Toc are in GPS SOW and do not include
         // the GPS week number.  OrbElem (rightly) insists on having a
         // Toe and Toc in CommonTime objects which implies determining
         // the week number.
      double timeDiff = Toe3 - XmitSOW;
      short epochWeek = fullXmitWeekNum;
      if (timeDiff < -HALFWEEK) epochWeek++;
      else if (timeDiff > HALFWEEK) epochWeek--;

         // End of Validity.
         // The end of validity is calculated from the fit interval
         // and the Toe.  Since this is RINEX, the fit interval is
         // already supposed to be stated in hours.
         // Round the Toe value to the hour to elminate confusion
         // due to possible "small offsets" indicating uploads
      short fitHours = fitDuration;
      long endFitSOW = Toe3 + (fitHours/2)*3600;
      short endFitWk = epochWeek;
      if (endFitSOW >= FULLWEEK)
      {
         endFitSOW -= FULLWEEK;
         endFitWk++;
      }

      switch (satID.system)
      {
         case SatelliteSystem::Glonass: 
         {
            ctToc        = GPSWeekSecond(epochWeek,       Toc3,       TimeSystem::GPS);
            ctToe        = GPSWeekSecond(epochWeek,       Toe3,       TimeSystem::GPS);
            beginValid   = GPSWeekSecond(fullXmitWeekNum, XmitSOW,   TimeSystem::GPS);
            transmitTime = GPSWeekSecond(fullXmitWeekNum, XmitSOW,   TimeSystem::GPS);
            endValid     = GPSWeekSecond(endFitWk,        endFitSOW, TimeSystem::GPS);
            break; 
         }

         case SatelliteSystem::BeiDou:  
         { 
            beginValid   = BDSWeekSecond(fullXmitWeekNum, XmitSOW,   TimeSystem::BDT);
            transmitTime = BDSWeekSecond(fullXmitWeekNum, XmitSOW,   TimeSystem::BDT);
            ctToc        = BDSWeekSecond(epochWeek,       Toc3,       TimeSystem::BDT);
            ctToe        = BDSWeekSecond(epochWeek,       Toe3,       TimeSystem::BDT);
            endValid     = BDSWeekSecond(endFitWk,        endFitSOW, TimeSystem::BDT);
            break; 
         }
         
         case SatelliteSystem::QZSS:    
         { 
            beginValid   = QZSWeekSecond(fullXmitWeekNum, XmitSOW,   TimeSystem::QZS);
            transmitTime = QZSWeekSecond(fullXmitWeekNum, XmitSOW,   TimeSystem::QZS);
            ctToc        = QZSWeekSecond(epochWeek,       Toc3,       TimeSystem::QZS);
            ctToe        = QZSWeekSecond(epochWeek,       Toe3,       TimeSystem::QZS);
            endValid     = QZSWeekSecond(endFitWk,        endFitSOW, TimeSystem::QZS);
            break; 
         }
         
         case SatelliteSystem::IRNSS:   
         { 
            beginValid   = IRNWeekSecond(fullXmitWeekNum, XmitSOW,   TimeSystem::IRN);
            transmitTime = IRNWeekSecond(fullXmitWeekNum, XmitSOW,   TimeSystem::IRN);
            ctToc        = IRNWeekSecond(epochWeek,       Toc3,       TimeSystem::IRN);
            ctToe        = IRNWeekSecond(epochWeek,       Toe3,       TimeSystem::IRN);
            endValid     = IRNWeekSecond(endFitWk,        endFitSOW, TimeSystem::IRN);
            break; 
         }
      }
   }

   //----------------------------------------------------------------
   // The following method should only be used by 
   // GPSOrbElemStore::rationnalize()
   void OrbElemRinex::adjustBeginningValidity()
   {
      if (!dataLoaded()) return;

         // The adjustment logic only applies to GPS.  The other
         // systems do not make these promises in their ICDs. 
      if (satID.system!=SatelliteSystem::GPS) 
         return;

         // The nominal beginning of validity is calculated from
         // the fit interval and the Toe.  In RINEX the fit duration
         // in hours is stored in the file.
      long  oneHalfInterval = ((long)fitDuration/2) * 3600;

         // If we assume this is the SECOND set of elements in a set
         // (which is an assumption of this function - see the .hpp) then
         // the "small offset in Toe" will actually push the Toe-oneHalfInterval
         // too early. For example, consider the following case.
         //         Toe : 19:59:44  (really near 20:00:00)
         //  first xMit : 18:00:00  (nominal)
         // Blindly setting beginValid top Toe - 1/2 fit interval will
         // result in 17:59:44.  But 18:00:00 actually is the right answer
         // because the -16 second offset is an artifact.
         //
         // Therefore, we are FIRST going to remove that offset,
         // THEN determine beginValid.
      long sow = (long) (static_cast<BDSWeekSecond>(ctToe)).sow;
      short week = (static_cast<GPSWeekSecond>(ctToe)).week;
      sow = sow + (3600 - (sow%3600));
      CommonTime adjustedToe = GPSWeekSecond(week, (double) sow);
      adjustedToe.setTimeSystem(TimeSystem::GPS);

      beginValid = adjustedToe - oneHalfInterval;
      return;
   }

   //----------------------------------------------------------------
   void OrbElemRinex::dumpHeader(ostream& s) const
   {
      if (!dataLoaded())
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      OrbElem::dumpHeader(s);

      s	<< "           SUBFRAME OVERHEAD"
      	<< endl
      	<< endl
      	<< "               SOW    DOW:HH:MM:SS     IOD\n";

      s << "   "
        << " HOW:   " << setw(7) << HOWtime
        << "  ";

      shortcut( s, HOWtime);
      s << "   ";

      s << "0x" << setfill('0') << hex;

      s << setw(3) << IODC;

      s << dec << "      " << setfill(' ');

      s << endl;


      s << endl
        << "           SV STATUS"
        << endl
        << endl
        << "Health bits         :      0x" << setfill('0') << hex << setw(2)
        << getHealth() << dec << ", " << getHealth();
      s << endl
        << "Fit duration (Hrs)  :         " << setw(1) << fitDuration << " hrs";
      s << endl
        << "Accuracy(m)         :      " << setfill(' ')
        << setw(4) << accuracyValue << " m" << endl
        << "Code on L2          :   ";

      switch (codeflags)
      {
         case 0:
            s << "reserved ";
            break;

         case 1:
            s << " P only  ";
            break;

         case 2:
            s << " C/A only";
            break;

         case 3:
            s << " P & C/A ";
            break;

         default:
            break;

      }
      s << endl
        <<"L2 P Nav data       :        ";
      if (L2Pdata!=0)
        s << "off";
      else
         s << "on";

      s.setf(ios::uppercase);
      s << endl;
      s << "Tgd                 : " << setw(13) << setprecision(6) << scientific << Tgd << " sec";
      s << endl;
   } // end of dumpHeader()

   //----------------------------------------------------------------
   void OrbElemRinex :: dumpTerse(ostream& s) const
   {

       // Check if the subframes have been loaded before attempting
       // to dump them.
      if (!dataLoaded())
      {
         InvalidRequest exc("No data in the object");
         GPSTK_THROW(exc);
      }

      ios::fmtflags oldFlags = s.flags();

      s.setf(ios::fixed, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(0);
      s.fill(' ');

      std::string svn;
      if (getSVN(satID, ctToe, svn))
      {
         s << setw(2) << " " << svn << "  ";
      }
      else
      {
         s << "      ";
      }

      s << setw(2) << satID.id << " ! ";

      string tform = "%3j %02H:%02M:%02S";

      s << printTime(beginValid, tform) << " ! ";
      s << printTime(ctToe, tform) << " ! ";
      s << printTime(endValid, tform) << " !  ";

      s << setw(4) << setprecision(1) << getAccuracy() << "  ! ";
      s << "0x" << setfill('0') << hex << setw(3) << IODC << " ! ";
      s << "0x" << setfill('0')  << setw(2) << getHealth();
      s << setfill(' ') << dec;
      s << "   " << setw(2) << getHealth() << " ! ";

      s << endl;
      s.flags(oldFlags);

    } // end of dumpTerse()


   //----------------------------------------------------------------
   void OrbElemRinex :: dump(ostream& s) const
   {
      ios::fmtflags oldFlags = s.flags();
      dumpHeader(s);
      dumpBody(s);
      s.flags(oldFlags);

   } // end of dump()

   //----------------------------------------------------------------
      // The following method is designed to work for all LNAV.  Therefore
      // it will be called by various descendents. 
      // It is a static implementation to allow unit tests apart from
      // building complete objects.
      // 
      // xmit - the transmisson time of the CEI data set.  For LNAV, this is the
      //        xmit time of the beginning of the first bit of the earliest message
      //        of the set. 
      // toe  - The toe of the data set.
   CommonTime OrbElemRinex::computeBeginValid(const SatID& satID,
                                              const CommonTime& xmit, 
                                              const CommonTime& ctToe )
   {
      int xmitWeek = static_cast<GPSWeekSecond>(xmit).week;
      long xmitSOW = (long) static_cast<GPSWeekSecond>(xmit).sow;

         // If the toe is NOT offset, then the begin valid time can be set
         // to the beginning of the two hour interval. 
         // NOTE: This is only true for GPS.   We can't do this
         // for QZSS, even though it also broadcasts the LNAV message format.
      if (satID.system==SatelliteSystem::GPS && isNominalToe(ctToe))
      {
         xmitSOW = xmitSOW - (xmitSOW % TWO_HOURS);
      }

         // If there IS an offset, all we can assume is that we (hopefullY)
         // captured the earliest transmission and set the begin valid time
         // to that value.
         //
         // NOTE: Prior to GPS III, the offset was typically applied to BOTH the first
         // and second data sets following a cutover.  So this means the SECOND data
         // set will NOT be coerced to the top of the even hour start time if it
         // wasn't collected at the top of the hour. 
      CommonTime beginValid = GPSWeekSecond(xmitWeek, xmitSOW, TimeSystem::GPS);
      return beginValid;
   } // end of computeBeginValid()

   //----------------------------------------------------------------
      // Launch of the first GPS III led to realization the end valid times have likely
      // been incorrect for some time.  There are two conditions.  
      // - The toe is in the nominal alignment.   In this case the mid-point of the 
      //   curve fit interval is aligned with the toe and the end valid determination
      //   is trivial. 
      // - The tow is NOT aligned with the nominal.   In this case, the mid-point of the 
      //   curve fit is the first even 15 minute interval later than the toe.
      //   Prior to GPS, this would be a 2 hour boundary.  With GPS III it could be
      //   any 15 minute boundary.
   CommonTime OrbElemRinex::computeEndValid(const CommonTime& ctToe,
                                            const int fitHours )
   {
         // Default case.
      long fitSeconds = fitHours * 3600;
      CommonTime endValid = ctToe + (double) (fitSeconds/2); 

         // If an upload cutover, need some adjustment.
         // Calculate the SOW aligned with the mid point and then
         // calculate the number of seconds the toe is SHORT
         // of that value.   That's how far the endValid needs
         // to be adjusted.   
      if (!isNominalToe(ctToe))
      {
         long sow = (long) static_cast<GPSWeekSecond>(ctToe).sow;
         long num900secIntervals = sow / 900;
         long midPointSOW = (num900secIntervals+1) * 900;
         double adjustUp = (double) (midPointSOW - sow);
         endValid += adjustUp;
      }
      return endValid;
   }

   //----------------------------------------------------------------
      // For a CEI data set that is NOT an upload cutover, toe should be 
      // an even two hour boundary.
   bool OrbElemRinex::isNominalToe(const gpstk::CommonTime& ctToe)
   {
      bool retVal = true;
      long toeSOW = (long) static_cast<gpstk::GPSWeekSecond>(ctToe).sow;
      long offsetFromEven2Hours = toeSOW % TWO_HOURS;
      if (offsetFromEven2Hours!=0)
         retVal = false;
      return retVal;
   }

   //----------------------------------------------------------------
   ostream& operator<<(ostream& s, const OrbElemRinex& eph)
   {
      try
      {
         eph.dump(s);
      }
      catch(gpstk::Exception& ex)
      {
         GPSTK_RETHROW(ex);
      }
      return s;

   } // end of operator<<

} // namespace
