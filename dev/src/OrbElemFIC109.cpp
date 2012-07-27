#pragma ident "$Id:$"

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2004, The University of Texas at Austin
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
 * @file OrbElemFIC109.cpp
 * Ephemeris data encapsulated in engineering terms
 */
#include <iomanip>
#include <cmath>

#include "OrbElemFIC109.hpp"
#include "StringUtils.hpp"
#include "GNSSconstants.hpp"
#include "GPS_URA.hpp"
#include "GPSWeekSecond.hpp"
#include "SVNumXRef.hpp"
#include "TimeString.hpp"
#include "EngNav.hpp"

namespace gpstk
{
   using namespace std;

   OrbElemFIC109::OrbElemFIC109()
   {
      AODO = 0;
      type = OrbElem::OrbElemFIC109;
   }

   OrbElemFIC109::OrbElemFIC109( const long SF1[10],
                                 const long SF2[10],
                                 const long SF3[10],
                                 const short PRNID,
                                 const short XmitGPSWeek )
      throw( InvalidParameter ) 
   {
      type = OrbElem::OrbElemFIC109;
      loadData(SF1, SF3, SF3, PRNID, XmitGPSWeek); 
   }
  
   OrbElemFIC109::OrbElemFIC109( const FICData& fic109 )
      throw( InvalidParameter )
   {
      type = OrbElem::OrbElemFIC109;
      loadData( fic109 );
   }

   OrbElemFIC109* OrbElemFIC109::clone() const
   {
      return new OrbElemFIC109 (*this); 
   }
   
   void OrbElemFIC109::loadData( const FICData& fic109)
      throw( InvalidParameter )
   {
      if (fic109.blockNum!=109)
      {
         InvalidParameter exc("Invalid FIC Block: "+StringUtils::asString(fic109.blockNum));
         GPSTK_THROW(exc);
      }
      short XmitGPSWeek = fic109.i[0];
      short PRNID = fic109.i[1];
      long SF1[10], SF2[10], SF3[10];
      for(int i = 0; i < 10; i++)
      {
         SF1[i] = fic109.i[2+i];
         SF2[i] = fic109.i[12+i];
         SF3[i] = fic109.i[22+i];
      }

      loadData( SF1,
                SF2,
                SF3,
                PRNID,
                XmitGPSWeek );
   }

   void OrbElemFIC109::loadData( const long SF1[10],
                                 const long SF2[10],
                                 const long SF3[10],
                                 const short PRNID,
                                 const short XmitGPSWeek )
      throw( InvalidParameter)
   {
      double ficOut[60];
      double ficTemp[60];
      EngNav engNavDummy; // Need this to initialize static data
      if(!EngNav::subframeConvert(SF1, XmitGPSWeek, ficTemp))
      {
         InvalidParameter exc("Invalid SF1 Data");
         GPSTK_THROW(exc);
      }
      for(int i = 0; i < 20; i++)
      {
         ficOut[i] = ficTemp[i];
      }
      if(!EngNav::subframeConvert(SF2, XmitGPSWeek, ficTemp))
      {
         InvalidParameter exc("Invalid SF2 Data");
         GPSTK_THROW(exc);
      }
      for(int i = 0; i < 20; i++)
      {
         ficOut[i+20] = ficTemp[i];
      }
      if(!EngNav::subframeConvert(SF3, XmitGPSWeek, ficTemp))
      {
         InvalidParameter exc("Invalid SF3 Data");
         GPSTK_THROW(exc);
      }
      for(int i = 0; i < 20; i++)
      {
         ficOut[i+40] = ficTemp[i];
      }
      ficOut[19] = PRNID;
   
      // Fill in the variables unique to OrbElemFIC109
      HOWtime[0] = static_cast<long>( ficOut[2] );
      ASalert[0] = static_cast<short>( ficOut[3] );
      codeflags  = static_cast<short>( ficOut[6] );
      accFlag    = static_cast<short>( ficOut[7] ); 
      health     = static_cast<short>( ficOut[8] );
      IODC       = static_cast<short>( ldexp( ficOut[9], -11 ) );
      L2Pdata    = static_cast<short>( ficOut[10] );
      Tgd        = ficOut[11];

      HOWtime[1]     = static_cast<long>( ficOut[22] );
      ASalert[1]     = static_cast<short>( ficOut[23] );
      IODE           = static_cast<short>( ldexp( ficOut[25], -11 ) );
      fitint         = static_cast<short>( ficOut[34] );

      HOWtime[2]       = static_cast<long>( ficOut[42] );
      ASalert[2]       = static_cast<short>( ficOut[43] );
      AODO             = static_cast<long>( ficOut[35] );

      short fullXmitWeekNum    = static_cast<short>( ficOut[5] );

         // Fill in the variables in the OrbElem parent
	 // - - - First the simple copies - - -
      double Toc     = ficOut[12];       // OrbElem only stores fully qualified times
                                         // see notes below.
      af2            = ficOut[13];
      af1            = ficOut[14];
      af0            = ficOut[15];

      Crs            = ficOut[26];
      dn             = ficOut[27];
      M0             = ficOut[28];
      Cuc            = ficOut[29];
      ecc            = ficOut[30];
      Cus            = ficOut[31];
      double AHalf   = ficOut[32];       // Not a member of OrbElem.  See notes below.
      double Toe     = ficOut[33];       // OrbElem only stores fully qualified times
                                         // see notes below.

      Cic            = ficOut[45];
      OMEGA0         = ficOut[46];
      Cis            = ficOut[47];
      i0             = ficOut[48];
      Crc            = ficOut[49];
      w              = ficOut[50];
      OMEGAdot       = ficOut[51];
      idot           = ficOut[53];
     
      // - - - Now work on the things that need to be calculated - - -

	 // The system is assumed (legacy navigation message is from GPS)
      satID.id = static_cast<short>( ficOut[19] );

         // The observation ID has a type of navigation, but the
         // carrier and code types are undefined.  They could be
         // L1/L2 C/A, P, Y,.....
      obsID.type = ObsID::otNavMsg;
      obsID.band = ObsID::cbUndefined;
      obsID.code = ObsID::tcUndefined;

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
	 //   2.) If Toc IS even two hour interval, pick time from SF 1, 
	 //   round back to nearest EVEN two hour boundary.  This assumes collection
	 //   SOMETIME in first hour of transmission.  Could be more
	 //   complete by looking at fit interval and IODC to more accurately 
	 //   determine earliest transmission time. 
      long longToc = (long) Toc;
      double XmitSOW = 0.0; 
      if ( (longToc % 7200) != 0)     // NOT an even two hour change
      {
         long leastHOW = HOWtime[0];
         if (HOWtime[1]<leastHOW) leastHOW = HOWtime[1];
         if (HOWtime[2]<leastHOW) leastHOW = HOWtime[2];	 
         long Xmit = leastHOW - (leastHOW % 30);
	 XmitSOW = (double) Xmit;
      }
      else
      {
         long Xmit = HOWtime[0] - HOWtime[0] % 7200;
	 XmitSOW = (double) Xmit; 
      }
      beginValid = GPSWeekSecond( fullXmitWeekNum, XmitSOW, TimeSystem::GPS ); 

      // Determine Transmit Time
      // Transmit time is the actual time this
      // SF 1/2/3 sample was collected
      long leastHOW = HOWtime[0];
      if (HOWtime[1]<leastHOW) leastHOW = HOWtime[1];
      if (HOWtime[2]<leastHOW) leastHOW = HOWtime[2];	 
      long Xmit = leastHOW - (leastHOW % 30);
      transmitTime = GPSWeekSecond( fullXmitWeekNum, (double)Xmit );

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

	 // End of Validity.  
	 // The end of validity is calculated from the fit interval
	 // and the Toe.  The fit interval is either trivial
	 // (if fit interval flag==0, fit interval is 4 hours) 
	 // or a look-up table based on the IODC. 
      short fitHours = getLegacyFitInterval(IODC, fitint);
      long endFitSOW = Toe + (fitHours/2)*3600;
      short endFitWk = epochWeek;
      if (endFitSOW >= FULLWEEK)
      {
         endFitSOW -= FULLWEEK;
         endFitWk++;
      }
      endValid = GPSWeekSecond(endFitWk, endFitSOW, TimeSystem::GPS);   

	 // Semi-major axis and time-rate-of-change of semi-major axis
	 //    Note: Legacy navigation message (SF 1/2/3) used SQRT(A).
	 //    The CNAV and CNAV-2 formats use deltaA and Adot.  As a 
	 //    result, OrbElem uses A and Adot and SQRT(A) and deltaA
	 //    are converted to A at runtime.
      A = AHalf * AHalf;
      Adot = 0.0; 
         // Legacy nav doesn't have Rate of Chagen to corerction to mean motion, 
	 // so set it to zero.
      dndot = 0.0;   

         // Health
         // OrbElemFIC109 stores the full 8 bits health from the legacy
	 // navigation message.  OrElemn only stores the true/false, 
	 // use/don't use based on whether the 8 bit health is 0 or non-zero
      healthy = true;
      if (health!=0) healthy = false;

         // URA Handling
      

         // After all this is done, declare that data has been loaded
	 // into this object (so it may be used). 
      dataLoaded = true; 

      return;
   }

   void OrbElemFIC109 :: dump(ostream& s) const
      throw( InvalidRequest )
   {
      if (!dataLoaded)
      {   
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      } 
      ios::fmtflags oldFlags = s.flags();
      s << "****************************************************************"
        << "************" << endl
        << "Broadcast Ephemeris (Engineering Units)";
      s << endl;
      s << "Source : FIC Block 109" << endl;
      OrbElemFIC9::dumpFIC9(s);
      s << "AODO                :     " << setw(5) << fixed << AODO << " sec" << endl;
      OrbElem::dump(s);
      s.flags(oldFlags);
   }    

   ostream& operator<<(ostream& s, const OrbElemFIC109& eph)
   {
      try
      {
         eph.dump(s);
      }
      catch(gpstk::Exception& ex)
      {
         ex.addLocation(FILE_LOCATION);
         GPSTK_RETHROW(ex);
      }
      return s;

   } // end of operator<<

  

} // namespace


