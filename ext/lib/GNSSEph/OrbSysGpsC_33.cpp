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
 * @file OrbSysGpsC_33.cpp
 * OrbSysGpsC_33 data encapsulated in engineering terms
 */
#include <iomanip>

#include "OrbSysGpsC_33.hpp"
#include "GPSWeekSecond.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"
#include "YDSTime.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   const double OrbSysGpsC_33::SIX_HOURS = 6.0 * 3600.0; 

   OrbSysGpsC_33::OrbSysGpsC_33()
      :OrbSysGpsC(),
       A0(0.0),
       A1(0.0),
       A2(0.0),
       dtLS(0), 
       dtLSF(0),
       DN(0),
       WN_LSF(0)
   {
   }

   OrbSysGpsC_33::OrbSysGpsC_33(const PackedNavBits& msg)
      throw( InvalidParameter):
      OrbSysGpsC()
   {
      loadData(msg);
   }

   OrbSysGpsC_33* OrbSysGpsC_33::clone() const
   {
      return new OrbSysGpsC_33 (*this); 
   }

   bool OrbSysGpsC_33::isSameData(const OrbData* right) const      
   {
         // First, test whether the test object is actually a OrbSysGpsC_33 object.
      const OrbSysGpsC_33* p = dynamic_cast<const OrbSysGpsC_33*>(right);
      if (p==0) return false; 

         // Establish if it refers to the same SV and UID. 
      if (!OrbSysGpsC::isSameData(right)) return false;
       
         // Finally, examine the contents
      if (ctEpoch != p->ctEpoch) return false;

      if (A0       !=p->A0)      return false;
      if (A1       !=p->A1)      return false;
      if (A2       !=p->A2)      return false;
      if (dtLS     !=p->dtLS)    return false;
      if (dtLSF    !=p->dtLSF)   return false;
      if (WN_LSF   !=p->WN_LSF)  return false;
      if (DN       !=p->DN)      return false; 
      return true;      
   }
   
   void OrbSysGpsC_33::loadData(const PackedNavBits& msg)
      throw(InvalidParameter)
   {
      setUID(msg);
      if (UID!=33)
      {
         char errStr[80];
         std::string msgString("Expected GPS CNAV MT 33.  Found unique ID ");
         msgString += StringUtils::asString(UID);
         InvalidParameter exc(msgString);
         GPSTK_THROW(exc);    
      } 
      obsID        = msg.getobsID();
      satID        = msg.getsatSys();
      beginValid   = msg.getTransmitTime();

      A0       = msg.asSignedDouble(127, 16, -35);
      A1       = msg.asSignedDouble(143, 13, -51);
      A2       = msg.asSignedDouble(156,  7, -68);

      dtLS   = (signed short) msg.asLong(163, 8, 1);

      unsigned long tot = msg.asUnsignedLong(171, 16, 16);
      unsigned int WNt = (unsigned int) msg.asUnsignedLong(187, 13,  1); 
      
      WN_LSF     = (unsigned short) msg.asUnsignedLong(200, 13, 1);
            
      DN      = (unsigned short) msg.asUnsignedLong(213, 4, 1);
      
      dtLSF  = (signed short) msg.asLong(217, 8, 1);
      
         // Deriving the epoch time is straight-forward given
         // the 13-bit week number.
      ctEpoch = GPSWeekSecond(WNt, tot, TimeSystem::GPS);

         // Note that DN parameter is 1-7
      double SOW = (DN-1) * gpstk::SEC_PER_DAY;
      ctLSF   = GPSWeekSecond(WN_LSF, SOW, TimeSystem::GPS);

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
      //
      // 2017/04/18 - OOOPS!  This is NOT what CNAV is doing. 
      // CNAV is broadcasting data with t-sub-ot in the past 
      // by about a day.   Therefore, we'll (a.) report this
      // to Karl and P.J., (b.) change this to be a straight
      // fit interval check.
   bool OrbSysGpsC_33::isUtcValid(const CommonTime& ct,
                                  const bool initialXMit) const
   {
      /*  Old test
         // Test that the t-sub-ot is in the future.  If 
         // initialXMit check that it is at least two days.
         // If not initial Xmit check that is is at least
         // one day. 
      double testDiff = 3600 * 48;
      if (!initialXMit) testDiff = 3600*24;
      double diff = ctEpoch - ct;

      //cout << " testDiff: " << testDiff << ", diff: " << diff << endl; 

      if (diff<testDiff) return false;

         // Test that the t-sub-ot is not more than a
         // week in the future.
      CommonTime testTime = ct + FULLWEEK;
      if (ctEpoch>testTime) return false;
      return true;
      */
      // Values from IS-GPS-200 Table 30-VIII
      double upperBound = (144 - 70) * 3600.0; 
      double lowerBound = -70 *3600; 

      // Determine distance in seconds between time of interest and t-sub-ot.  
      // Positive is when the time of interest is in the future wrt to the t-sub-ot
      double testDiff = ct - ctEpoch;
      if (testDiff>=lowerBound &&
          testDiff<=upperBound) return true;
      return false; 
   } // end of isUtcValid()

      // 20.3.3.5.2.4 establishes three cases. Before, near the event, 
      // and after a leap second.  Unfortunately, in the middle case,
      // the interface specification is complicated by the fact it
      // is working in SOW and there is a need to account for
      // week rollovers. 
   double OrbSysGpsC_33::getUtcOffset(const CommonTime& ct) const
   {
      double retVal;

         // delta t-sub-UTC is the same in all cases.
      double dtUTC = getUtcOffsetModLeapSec(ct);

         // ctLSF is the "GPS day" that will contain the leap second.   In addition, ctLSF is
         // set to the BEGINNING of the day that will contain the leap second.   
         // First, compute the offset between the GPS time of interest and the 
         // time of effectivity of the leap second (in GPS time)
      double diff = (ctLSF + SEC_PER_DAY) - ct; 
      double diffAbs = fabs(diff);
      double SEC_PER_HALF_DAY = SEC_PER_DAY / 2.0; 

      //cout << " diff, diffAbs : " << diff << ", " << diffAbs << endl;

         // Case a: Effectivity is NOT in the past and is more than
         //         six hours in the future
      if (diff>0 && diffAbs>SEC_PER_HALF_DAY)
      {
         //cout << "Case a" << endl;
         retVal = (double) dtLS + dtUTC; 
      }
         // Case c: Effectivity is in the past and more than a 
         // half-day in the past. 
      else if (diff<0 && diffAbs>SEC_PER_HALF_DAY)
      {
         //cout << "Case c" << endl;
         retVal = (double) dtLSF + dtUTC;
      }
         //  What remains is case b.
         //  This attempts to implement what's in IS-GPS-200 as closely as practical.  
         //  In point of fact, I needed to put together a spreadsheet implementation of 
         //  case b before I could understand it. 
      else
      {
         //cout << "Case b" << endl;
         long SOD =  static_cast<YDSTime>(ct).sod;

         long variableModulo = 86400 + dtLSF - dtLS;
         
         double WLeftTerm = (SOD - ((double) dtLS + dtUTC)) - 43200;
         double W = fmod(WLeftTerm,86400.0);

            // Modulo SHOULD return [0.0, +value} but appears to return (-value, value)
         if (W<0.0) W += SEC_PER_DAY;         
         W += 43200;

         double SODutc = fmod(W,variableModulo); 

         double deltaSOD = SODutc - SOD;
         if (deltaSOD<-SEC_PER_HALF_DAY)
            deltaSOD += SEC_PER_DAY;
         else if (deltaSOD>SEC_PER_HALF_DAY) 
            deltaSOD -= SEC_PER_DAY;

         CommonTime ctUTC = ct + deltaSOD;

         //debug
         //cout << " SOD, WLeftTerm, W, variableMod, SODutc, deltaSOD:  " << SOD << ", " << WLeftTerm << ", "
         //     << W << ", " << variableModulo << ", " << SODutc << ", " << deltaSOD << endl;

         // We're supposed to return delta t-sub-UTC, so we have to do a little manipulation.
         //   t-sub-UTC = t-sub-E - delta t-sub-UTC. 
         //   delta t-sub-UTC = t-sub-E - t-sub-UTC 
         retVal = ct - ctUTC;
      }

      return retVal;
   } // end of getUtcOffset()

   double OrbSysGpsC_33::getUtcOffsetModLeapSec(const CommonTime& ct) const
   {
      double retVal;
      double dt = ct - ctEpoch;
      retVal = A0 + (A1 * dt) + (A2 * dt * dt);
      return retVal;
   } // end of getUtcOffsetModLeapSec()


   void OrbSysGpsC_33::dumpUtcTerse(std::ostream& s, const std::string tform) const
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

      s << "  33";      // UID
      s << " " << printTime(beginValid,tform) << "  ";
      s << "tot: " << printTime(ctEpoch,tform) << " "; 

      s.setf(ios::scientific, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.precision(10);
      s.fill(' ');

      s << " A0:" << setw(18) << A0 << " ";
      s << " A1:" << setw(18) << A1 << " ";
      s << " A2:" << setw(18) << A2 << " ";

      s.setf(ios::fixed, ios::floatfield);
      s.precision(0);
      s << " dtLS:" << setw(4) << dtLS;
   } // end of dumpTerse()

   void OrbSysGpsC_33::dumpBody(ostream& s) const
      throw( InvalidRequest )
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      s << endl
        << "           GPS UTC PARAMETERS"
        << endl
        << "Parameter              Value" << endl;

      string tform="  %02m/%02d/%04Y %02H:%02M:%02S  Week %F  SOW %6.0g";
      s << "t-sub-ot    " << printTime(ctEpoch,tform) << endl; 

      s.setf(ios::scientific, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.precision(10);
      s.fill(' ');
    
      s << "A0         " << setw(17) << A0 << " sec" << endl;
      s << "A1         " << setw(17) << A1 << " sec/sec" << endl;
      s << "A2         " << setw(17) << A2 << " sec/sec**2" << endl;

      s.setf(ios::fixed, ios::floatfield);
      s.precision(0);
      s << "dtLS        " << setw(16) << dtLS << " sec" << endl;
      s << "dtLSF       " << setw(16) << dtLSF << " sec" << endl;
      s << "WN_LSF      " << setw(11) << WN_LSF 
                          << " Full GPS week" << endl;
      s << "DN          " << setw(16)<<  DN << " day (1-7)" << endl;
      s << "Epoch(lsf)        " << printTime(ctLSF,"%02m/%02d/%04Y") << endl;

      
   } // end of dumpBody()   

} // end namespace gpstk
