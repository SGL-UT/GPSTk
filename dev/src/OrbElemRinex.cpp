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
 * @file OrbElemRinex.cpp
 * Ephemeris data encapsulated in engineering terms
 */
#include <iomanip>
#include <cmath>

#include "OrbElemRinex.hpp"
#include "StringUtils.hpp"
#include "GNSSconstants.hpp"
#include "GPS_URA.hpp"
#include "GPSWeekSecond.hpp"
#include "SVNumXRef.hpp"
#include "TimeString.hpp"

namespace gpstk
{
   using namespace std;
  
   OrbElemRinex::OrbElemRinex()
   {
      dataLoaded = false;     
      codeflags = health = L2Pdata = 0;

      accuracyValue = 0.0;

      IODC = 0;
      Tgd = 0.0;
      type = OrbElem::OrbElemRinex;
      fitDuration = 0;
   }

  

   OrbElemRinex::OrbElemRinex( const RinexNavData& rinNav )
      throw( InvalidParameter )
   {
      type = OrbElem::OrbElemRinex;
      loadData( rinNav );
   }

     /// Clone method
   OrbElemRinex* OrbElemRinex::clone() const
   {
      return new OrbElemRinex (*this);
   } 

   void OrbElemRinex::loadData( const RinexNavData& rinNav )
      throw( InvalidParameter )
   {
      // Fill in the variables unique to OrbElemFIC9
      codeflags        = rinNav.codeflgs;
      accuracyValue    = rinNav.accuracy; 
      health           = rinNav.health;
      IODC             = rinNav.IODC;
      L2Pdata          = rinNav.L2Pdata;
      Tgd              = rinNav.Tgd;

      HOWtime        = rinNav.HOWtime;
      fitDuration    = rinNav.fitint;

      short fullXmitWeekNum    = rinNav.weeknum;

         // Fill in the variables in the OrbElem parent
	 // - - - First the simple copies - - -
      double Toc     = rinNav.Toc;       // OrbElem only stores fully qualified times
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
      
                                         
      double Toe     = rinNav.Toe;       // Not a member of OrbElem.  See notes below.
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
      satID.id = static_cast<short>( rinNav.PRNID );

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
         long Xmit = HOWtime - (HOWtime % 30);
	 XmitSOW = (double) Xmit;
      }
      else
      {
         long Xmit = HOWtime - HOWtime % 7200;
	 XmitSOW = (double) Xmit; 
      }
      beginValid = GPSWeekSecond( fullXmitWeekNum, XmitSOW, TimeSystem::GPS ); 

      // Determine Transmit Time
      // Transmit time is the actual time this
      // SF 1/2/3 sample was collected	 
      long Xmit = HOWtime - (HOWtime % 30);
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

	 // End of Validity.  
	 // The end of validity is calculated from the fit interval
	 // and the Toe.  The fit interval is either trivial
	 // (if fit interval flag==0, fit interval is 4 hours) 
	 // or a look-up table based on the IODC. 
      short fitHours = getLegacyFitInterval(IODC, fitDuration);
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
         // OrbElemFIC9 stores the full 8 bits health from the legacy
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

   bool OrbElemRinex::hasData( ) const
      { return( dataLoaded ); }

   double OrbElemRinex::getAccuracy()  const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return ( accuracyValue );
   }   

   void OrbElemRinex :: dumpTerse(ostream& s) const
      throw(InvalidRequest )
   {
     
       // Check if the subframes have been loaded before attempting
       // to dump them.
      if (!dataLoaded)
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

      SVNumXRef svNumXRef; 
      int NAVSTARNum = 0;
      try
      {
	NAVSTARNum = svNumXRef.getNAVSTAR(satID.id, ctToe );
        
     
        s << setw(2) << " " << NAVSTARNum << "  ";
      }
      catch(SVNumXRef::NoNAVSTARNumberFound)
      { 
	s << "  XX  ";
      }

      s << setw(2) << satID.id << " ! ";
      
      string tform = "%3j %02H:%02M:%02S";

      s << printTime(beginValid, tform) << " ! ";
      s << printTime(ctToe, tform) << " ! ";
      s << printTime(endValid, tform) << " !  ";

      s << setw(4) << setprecision(1) << getAccuracy() << "  ! ";
      s << "0x" << setfill('0') << hex << setw(3) << IODC << " ! ";
      s << "0x" << setfill('0')  << setw(2) << health;
      s << setfill(' ') << dec;
      s << "   " << setw(2) << health << " ! ";

      s << endl;
      s.flags(oldFlags);

    } // end of dumpTerse()
  
   static void shortcut(ostream & os, const long HOW )
   {
      short DOW, hour, min, sec;
      long SOD, SOW;
      short SOH;

      SOW = static_cast<long>( HOW );
      DOW = static_cast<short>( SOW / SEC_PER_DAY );
      SOD = SOW - static_cast<long>( DOW * SEC_PER_DAY );
      hour = static_cast<short>( SOD/3600 );

      SOH = static_cast<short>( SOD - (hour*3600) );
      min = SOH/60;

      sec = SOH - min * 60;
      switch (DOW)
      {
         case 0: os << "Sun-0"; break;
         case 1: os << "Mon-1"; break;
         case 2: os << "Tue-2"; break;
         case 3: os << "Wed-3"; break;
         case 4: os << "Thu-4"; break;
         case 5: os << "Fri-5"; break;
         case 6: os << "Sat-6"; break;
         default: break;
      }

      os << ":" << setfill('0')
         << setw(2) << hour
         << ":" << setw(2) << min
         << ":" << setw(2) << sec
         << setfill(' ');
   } 
   
 

   void OrbElemRinex :: dump(ostream& s) const
      throw( InvalidRequest )
   {     
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      ios::fmtflags oldFlags = s.flags();
      
      SVNumXRef svNumXRef; 
      int NAVSTARNum = 0; 

      s << "****************************************************************"
        << "************" << endl
        << "Broadcast Ephemeris (Engineering Units)";
      s << endl;

      s << "Source : Rinex Navigation Message File" << endl;
 
      s << endl;
      s << "PRN : " << setw(2) << satID.id << " / "
        << "SVN : " << setw(2);
      try
      {
	NAVSTARNum = svNumXRef.getNAVSTAR(satID.id, ctToe );
        s << NAVSTARNum << "  ";
      }
      catch(SVNumXRef::NoNAVSTARNumberFound)
      { 
	s << "XX";
      }
         

      s << endl
        << endl
      	<< "           SUBFRAME OVERHEAD"
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
        << "Health bits         :      0x" << setfill('0')  << setw(2) << health;
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
           
      OrbElem::dump(s);
      s.flags(oldFlags);

   } // end of dump()

  

   ostream& operator<<(ostream& s, const OrbElemRinex& eph)
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
