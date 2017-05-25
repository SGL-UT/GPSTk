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
 * @file OrbSysGpsC_32.cpp
 * CNAV MT 32 data encapsulated in engineering terms
 */
#include <iomanip>

#include "OrbSysGpsC_32.hpp"
#include "GPSWeekSecond.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   OrbSysGpsC_32::OrbSysGpsC_32()
      :OrbSysGpsC(),
       PM_X(0.0),
       PM_X_dot(0.0),
       PM_Y(0.0),
       PM_Y_dot(0.0),
       delta_UT1(0.0),
       delta_UT1_dot(0.0)
   {
   }

   OrbSysGpsC_32::OrbSysGpsC_32(const PackedNavBits& msg)
      throw( InvalidParameter):
      OrbSysGpsC()
   {
      loadData(msg);
   }

   OrbSysGpsC_32* OrbSysGpsC_32::clone() const
   {
      return new OrbSysGpsC_32 (*this); 
   }

   bool OrbSysGpsC_32::isSameData(const OrbData* right) const      
   {
         // First, test whether the test object is actually a OrbSysGpsC_32 object.
      const OrbSysGpsC_32* p = dynamic_cast<const OrbSysGpsC_32*>(right);
      if (p==0) return false; 

         // Establish if it refers to the same SV and UID. 
      if (!OrbSysGpsC::isSameData(right)) return false;
       
         // Finally, examine the contents
      if (ctEpoch != p->ctEpoch) return false;

      if (PM_X          !=p->PM_X)          return false;
      if (PM_X_dot      !=p->PM_X_dot)      return false;
      if (PM_Y          !=p->PM_Y)          return false;
      if (PM_Y_dot      !=p->PM_Y_dot)      return false;
      if (delta_UT1     !=p->delta_UT1)     return false;
      if (delta_UT1_dot !=p->delta_UT1_dot) return false;

      return true;      
   }
   
   void OrbSysGpsC_32::loadData(const PackedNavBits& msg)
      throw(InvalidParameter)
   {
      setUID(msg);
      if (UID!=32)
      {
         char errStr[80];
         std::string msgString("Expected GPS CNAV MT 32.  Found unique ID ");
         msgString += StringUtils::asString(UID);
         InvalidParameter exc(msgString);
         GPSTK_THROW(exc);    
      } 
      obsID        = msg.getobsID();
      satID        = msg.getsatSys();
      beginValid   = msg.getTransmitTime();

      tEOP           = msg.asUnsignedLong(127, 16, 16);
      PM_X           = msg.asSignedDouble(143, 21, -20);
      PM_X_dot       = msg.asSignedDouble(164, 15, -21);
      PM_Y           = msg.asSignedDouble(179, 21, -20);
      PM_Y_dot       = msg.asSignedDouble(200, 15, -21);
      delta_UT1      = msg.asSignedDouble(215, 31, -24);
      delta_UT1_dot  = msg.asSignedDouble(246, 19, -25);
      delta_UT1_dot_per_sec = delta_UT1_dot / SEC_PER_DAY;
   
         // Deriving the epoch time is challenging due to the
         // lack of a week number in this message. For the moment, 
         // assume tEOP is within a half-week of the transmit time.
      unsigned short WNxmit = static_cast<GPSWeekSecond>(beginValid).week; 
      double SOWxmit = static_cast<GPSWeekSecond>(beginValid).sow; 
      long lSOWxmit = (unsigned long) SOWxmit; 
      unsigned short WN = WNxmit;
      long diffSOW = tEOP - lSOWxmit;
      if (diffSOW>HALFWEEK) WN--;
       else if (diffSOW<-HALFWEEK) WN++;

      ctEpoch = GPSWeekSecond(WN, tEOP, TimeSystem::GPS);

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
   bool OrbSysGpsC_32::isEopValid(const CommonTime& ct,
                                  const bool initialXMit) const
   {
         // Test that the t-sub-EOP is in the future.  If 
         // initialXMit check that it is at least two days.
         // If not initial Xmit check that is is at least
         // one day. 
      double testDiff = 3600 * 48;
      if (!initialXMit) testDiff = 3600*24;
      double diff = ctEpoch - ct;

      //cout << " testDiff: " << testDiff << ", diff: " << diff << endl; 

      if (diff<testDiff) return false;

         // Test that the t-sub-EOP is not more than a
         // week in the future.
      CommonTime testTime = ct + FULLWEEK;
      if (ctEpoch>testTime) return false;
      return true;
   } // end of isEopValid()

   CommonTime OrbSysGpsC_32::getUT1(const CommonTime& ct, 
                                const CommonTime& tutc) const
      throw( InvalidRequest )
   {
      if (tutc.getTimeSystem()!=TimeSystem::UTC)
      {
         stringstream ss;
         ss << "in OrbSysGpsC_32.  TimeSystem of tutc parameter must be";
         ss << " TimeSystem::UTC";
         InvalidRequest ir(ss.str());
         GPSTK_THROW(ir);
      }

      double elapt = ct - ctEpoch; 
      double UT1Adjust = delta_UT1 + delta_UT1_dot_per_sec * elapt;

      CommonTime retVal = tutc;
      retVal += UT1Adjust;

      return retVal;
   }

   CommonTime OrbSysGpsC_32::getUT1(const CommonTime& ct,
                                const OrbSysGpsC_33* mt33) const
      throw( InvalidRequest )
   {
      if (!mt33->isUtcValid(ct))
      {
         string tform = "%02m/%02d/%04Y %02H:%02M:%02S";
         stringstream ss;
         ss << "Time of ";
         ss << printTime(ct,tform);
         ss << " is not a valid evaluation time for an MT33 with a t-sub-ot of ";
         ss << printTime(mt33->ctEpoch,tform);
         InvalidRequest ir(ss.str());
         GPSTK_THROW(ir); 
      }

      // The simple approach would be 
      //   double utcOffset = mt33.getUtcOffset(ct);
      //
      // However, in the event of a leap second adjust, the UTC would have a
      // discontinuity and therefore the UT1 would have a discontinuity.   Therefore,
      // we have to compute the UTC offset from the MT 33 contents and use 
      // delta t-sub-LS regardless of the leap second situation.
               // delta t-sub-UTC is the same in all cases.
      double dtUTC = mt33->getUtcOffsetModLeapSec(ct);
      double utcOffset = (double) mt33->dtLS + dtUTC; 

      CommonTime tutc = ct - utcOffset;
      tutc.setTimeSystem(TimeSystem::UTC); 

      return getUT1(ct, tutc);
   }


   double OrbSysGpsC_32::getxp(const CommonTime& ct) const
   {
      double elapt = ct - ctEpoch; 
      double retVal = PM_X + PM_X_dot/SEC_PER_DAY * elapt;
      return retVal; 
   }

   double OrbSysGpsC_32::getyp(const CommonTime& ct) const
   {
      double elapt = ct - ctEpoch; 
      double retVal = PM_Y + PM_Y_dot/SEC_PER_DAY * elapt;
      return retVal; 
   }

   void OrbSysGpsC_32::dumpTerse(std::ostream& s) const
         throw(InvalidRequest)
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      string tform = "%02m/%02d/%04Y %03j %02H:%02M:%02S"; 
      string ssys = SatID::convertSatelliteSystemToString(satID.system); 
      s << setw(7) << ssys;
      s << " " << setw(2) << satID.id;

      s << "  32";      // UID
      s << " " << printTime(beginValid,tform) << "  ";
      s << "tEOP: " << printTime(ctEpoch,tform) << " "; 

      s.setf(ios::scientific, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(10);
      s.fill(' ');

      s << " PM_X:" << setw(18) << PM_X << " ";
      s << " PM_Y:" << setw(18) << PM_Y << " ";
      s << " dUT1:" << setw(18) << delta_UT1 << " ";

   } // end of dumpTerse()

   void OrbSysGpsC_32::dumpBody(ostream& s) const
      throw( InvalidRequest )
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }

      s << endl
        << "           EOP PARAMETERS"
        << endl
        << "Parameter              Value" << endl;

      string tform="  %02m/%02d/%04Y %02H:%02M:%02S";
      s << "t-sub-EOP     " << printTime(ctEpoch,tform) << endl; 
      s.setf(ios::fixed, ios::floatfield);
      s.precision(0);
      s << "t-sub_EOP(sow)" << setw(10) << tEOP << " sec" << endl;

      s.setf(ios::scientific, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(10);
      s.fill(' ');
      s << "PM_X          " << setw(16) << PM_X << " arc -sec" << endl;
      s << "PM_X_dot      " << setw(16) << PM_X_dot << " arc-sec/day" << endl;
      s << "PM_Y          " << setw(16) << PM_Y << " arc-sec" << endl;
      s << "PM_Y_dot      " << setw(16) << PM_Y_dot << " arc-sec/day" << endl;
      s << "delta_UT1     " << setw(16) << delta_UT1 << " sec" << endl;
      s << "delta_UT1_dot " << setw(16) << delta_UT1_dot << " sec/day" << endl;

      
   } // end of dumpBody()   

} // end namespace gpstk
