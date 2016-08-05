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
 * @file OrbSysGpsL_56.cpp
 * OrbSysGpsL_56 data encapsulated in engineering terms
 */
#include <iomanip>

#include "OrbSysGpsL_56.hpp"
#include "GPSWeekSecond.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   const double OrbSysGpsL_56::SIX_HOURS = 6.0 * 3600.0; 

   OrbSysGpsL_56::OrbSysGpsL_56()
      :OrbSysGpsL(),
       A0(0.0),
       A1(0.0),
       dtLS(0), 
       dtLSF(0),
       WN_LSF(0),
       DN(0),
       WN_LSF_full(0)
   {
      for (int i=0;i<4;i++) { alpha[i] = 0.0; beta[i] = 0.0; }
   }

   OrbSysGpsL_56::OrbSysGpsL_56(const PackedNavBits& msg)
      throw( InvalidParameter):
      OrbSysGpsL()
   {
      loadData(msg);
   }

   OrbSysGpsL_56* OrbSysGpsL_56::clone() const
   {
      return new OrbSysGpsL_56 (*this); 
   }

   bool OrbSysGpsL_56::isSameData(const OrbData* right) const      
   {
         // First, test whether the test object is actually a OrbSysGpsL_56 object.
      const OrbSysGpsL_56* p = dynamic_cast<const OrbSysGpsL_56*>(right);
      if (p==0) return false; 

         // Establish if it refers to the same SV and UID. 
      if (!OrbSysGpsL::isSameData(right)) return false;
       
         // Finally, examine the contents
      if (ctEpoch != p->ctEpoch) return false;

      for (int i=0; i<4; i++)
      {
         if (alpha[i] != p->alpha[i]) return false;
         if (beta[i]  != p->beta[i]) return false;
      }
      if (A0       !=p->A0)      return false;
      if (A1       !=p->A1)      return false;
      if (dtLS     !=p->dtLS)    return false;
      if (dtLSF    !=p->dtLSF)   return false;
      if (WN_LSF   !=p->WN_LSF)  return false;
      if (DN       !=p->DN)      return false; 
      return true;      
   }
   
   void OrbSysGpsL_56::loadData(const PackedNavBits& msg)
      throw(InvalidParameter)
   {
      setUID(msg);
      if (UID!=56)
      {
         char errStr[80];
         std::string msgString("Expected GPS Subframe 4, Page 18, SVID 56 (418).  Found unique ID ");
         msgString += StringUtils::asString(UID);
         InvalidParameter exc(msgString);
         GPSTK_THROW(exc);    
      } 
      obsID        = msg.getobsID();
      satID        = msg.getsatSys();
      beginValid   = msg.getTransmitTime();

      alpha[0] = msg.asSignedDouble(68, 8, -30);
      alpha[1] = msg.asDoubleSemiCircles(76, 8, -27);
      alpha[2] = msg.asDoubleSemiCircles(90, 8, -24);
      alpha[3] = msg.asDoubleSemiCircles(98, 8, -24);

      beta[0]  = msg.asSignedDouble(106, 8, 11);
      beta[1]  = msg.asDoubleSemiCircles(120, 8, 14);
      beta[2]  = msg.asDoubleSemiCircles(128, 8, 16);
      beta[3]  = msg.asDoubleSemiCircles(136, 8, 16);

      A1       = msg.asSignedDouble(150, 24, -50);
      
      const unsigned startBits1[] = {180, 210};
      const unsigned numBits1[]   = {24,  8};
      A0       = msg.asSignedDouble(startBits1, numBits1, 2, -30);

      unsigned long tot = msg.asUnsignedLong(218, 8, 4096);
      unsigned int WNt = (unsigned int) msg.asUnsignedLong(226, 8,  1); 

      dtLS   = (signed short) msg.asLong(240, 8, 1);
      
      WN_LSF     = (unsigned short) msg.asUnsignedLong(248, 8, 1);
            
      DN      = (unsigned short) msg.asUnsignedLong(256, 8, 1);
      
      dtLSF  = (signed short) msg.asLong(270, 8, 1);
      
         // Deriving the epoch time is a bit of a process.
         // WNt and WN_LSF are both 8 bit modulo 256 values that
         // represent the lower 8 bits of the GPS week numbers and
         // are assumed to be within 127 weeks of the current 
         // GPS week.  
         // Therefore, to determine the WN of the epoch time 
         //    We retrieve the current WN from the transmit time.
         //    We form the modulo 256 remainder of WN
         //    Form the differenece of the WNmod and the WNt.
         //      Usually, WNt will be same or in the future.
         //      So this value should be 0 or positive.  
         //      In any event, it will be in the range -128 to +127.
         // 
      unsigned short WN = static_cast<GPSWeekSecond>(beginValid).week; 
      unsigned short WNmod = WN % 256;
      short diff = WNt - WNmod; 
      if (diff<-128) diff += 256;
      if (diff>127) diff -= 256; 
      unsigned short WNt_full = WN + diff; 
      ctEpoch = GPSWeekSecond(WNt_full, tot, TimeSystem::GPS);
/*
      cout << " WN, WNmod, WNt, diff, WNt_full: " << WN
           << ", " << WNmod << ", " << WNt
           << ", " << diff << ", " << WNt_full << endl;
      cout << "tot = " << tot << endl;
      cout << printTime(ctEpoch,"%02m/%02d/%04Y %02H:%02M:%02S") << endl;
*/

         // As above for forming the epoch time for the future leap second. 
      diff = WN_LSF - WNmod;
      if (diff<-128) diff += 256;
      if (diff>127)  diff -= 256;
      WN_LSF_full = WN + diff;

         // Note that DN parameter is 1-7
      double SOW = (DN-1) * gpstk::SEC_PER_DAY;
      ctLSF   = GPSWeekSecond(WN_LSF_full, SOW, TimeSystem::GPS);

      dataLoadedFlag = true;   
   } // end of loadData()


      // Test that epoch time meets the criteria
      // xmit Time < epoch time < (xmit Time + 1 week)
      //
      // This test is based on 20.3.3.3.5.4.a (last paragraph)
      // and Karl Kovach's interpretation thereof following 
      // the time anomaly of 1/25-26/2016.
      // 1.) t-sub-ot must be in the future from
      //     the provided time.
      // 2.) t-sub-ot must be less than a week in the 
      //     future from the provided time.  
      // 
      // Note that if initialXMit is false (default) the 
      // following interpretation applies:  It is assumed
      // that the transmit interval for the data is approximately
      // 24 hours.  Therefore, t-sub-ot is still in the future
      // at the end of the transmission interval, but may 
      // only be in the future by ~(70-24) hours = 46 hours. 
   bool OrbSysGpsL_56::isUtcValid(const CommonTime& ct,
                                  const bool initialXMit) const
   {
         // Test that the t-sub-ot is in the future.  If 
         // initialXMit check that it is at least two days.
         // If not initial Xmit check that is is at least
         // one day. 
      double testDiff = 3600 * 48;
      if (!initialXMit) testDiff = 3600*24;
      double diff = ctEpoch - ct;

      cout << " testDiff: " << testDiff << ", diff: " << diff << endl; 

      if (diff<testDiff) return false;

         // Test that the t-sub-ot is not more than a
         // week in the future.
      CommonTime testTime = ct + FULLWEEK;
      if (ctEpoch>testTime) return false;
      return true;
   } // end of isUtcValid()

      // 20.3.3.5.2.4 establishes three cases. Before, near the event, 
      // and after a leap second.  Unfortunately, in the middle case,
      // the interface specification is complicated by the fact it
      // is working in SOW and there is a need to account for
      // week rollovers. 
   double OrbSysGpsL_56::getUtcOffset(const CommonTime& ct) const
   {
      double retVal;

         // delta t-sub-UTC is the same in all cases.
      double dtUTC = getUtcOffsetModLeapSec(ct);

         // compute offset between user's time and 
         // leap second time of effectivity. 
      double diff = ctLSF - ct; 
      double diffAbs = fabs(diff);

         // Case a: WN-sub-LSF/DN is not in the past.
         // That is to say, it is >= the current time. 
      if (diff>=0)
      {
         retVal = (double) dtLS + dtUTC; 
      }
      else
      {
         retVal = (double) dtLSF + dtUTC;
      }

      return retVal;
   } // end of getUtcOffset()

   double OrbSysGpsL_56::getUtcOffsetModLeapSec(const CommonTime& ct) const
   {
      double retVal;
      retVal = A0 + A1 * (ct - ctEpoch);
      return retVal;
   } // end of getUtcOffsetModLeapSec()


   void OrbSysGpsL_56::dumpUtcTerse(std::ostream& s, const std::string tform) const
         throw(InvalidRequest)
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      string ssys = SatID::convertSatelliteSystemToString(satID.system); 
      s << setw(7) << ssys;
      s << " " << setw(2) << satID.id;

      s << "  56";      // UID
      s << " " << printTime(beginValid,tform) << "  ";
      s << "tot: " << printTime(ctEpoch,tform) << " "; 

      s.setf(ios::scientific, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(10);
      s.fill(' ');

      s << " A0:" << setw(18) << A0 << " ";
      s << " A1:" << setw(18) << A1 << " ";

      s.setf(ios::fixed, ios::floatfield);
      s.precision(0);
      s << " dtLS:" << setw(4) << dtLS;
   } // end of dumpTerse()

   void OrbSysGpsL_56::dumpBody(ostream& s) const
      throw( InvalidRequest )
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      s << endl
        << "           GPS IONO PARAMETERS"
        << endl
        << "Parameter              Value" << endl;

      s.setf(ios::scientific, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(8);
      s.fill(' ');
      s << "alpha[0]    " << setw(16) << alpha[0] << " sec" << endl;
      s << "alpha[1]    " << setw(16) << alpha[1] << " sec/rad" << endl;
      s << "alpha[2]    " << setw(16) << alpha[2] << " sec/rad**2" << endl;
      s << "alpha[3]    " << setw(16) << alpha[3] << " sec/rad**3" << endl;
      s << "beta[0]     " << setw(16) << beta[0] << " sec" << endl;
      s << "beta[1]     " << setw(16) << beta[1] << " sec/rad" << endl;
      s << "beta[2]     " << setw(16) << beta[2] << " sec/rad**2" << endl;
      s << "beta[3]     " << setw(16) << beta[3] << " sec/rad**3" << endl;

      s << endl
        << "           GPS UTC PARAMETERS"
        << endl
        << "Parameter              Value" << endl;

      string tform="  %02m/%02d/%04Y %02H:%02M:%02S";
      s << "t-sub-ot    " << printTime(ctEpoch,tform) << endl; 
      s << "A0          " << setw(16) << A0 << " sec" << endl;
      s << "A1          " << setw(16) << A1 << " sec/sec" << endl;

      s.setf(ios::fixed, ios::floatfield);
      s.precision(0);
      s << "dtLS        " << setw(16) << dtLS << " sec" << endl;
      s << "dtLSF       " << setw(16) << dtLSF << " sec" << endl;
      s << "WN_LSF      " << setw(11) << WN_LSF_full 
                          << "(" << setw(3) << WN_LSF << ")" << " Full week (modulo 256 week)" << endl;
      s << "DN          " << setw(16)<<  DN << " day (1-7)" << endl;
      s << "Epoch(lsf)        " << printTime(ctLSF,"%02m/%02d/%04Y") << endl;

      
   } // end of dumpBody()   

} // end namespace gpstk
