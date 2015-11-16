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

#include "TimeSystemCorr.hpp"

namespace gpstk
{
   TimeSystemCorrection ::
   TimeSystemCorrection()
         : type(Unknown), frTS(TimeSystem::Unknown), toTS(TimeSystem::Unknown)
   {
   }


   TimeSystemCorrection ::
   TimeSystemCorrection(std::string str)
   {
      this->fromString(str);
   }


   void TimeSystemCorrection ::
   fromString(const std::string& str)
   {
      std::string STR(gpstk::StringUtils::upperCase(str));
      if(STR == std::string("GPUT"))
      {
         type = GPUT;
         frTS = TimeSystem::GPS;
         toTS = TimeSystem::UTC;
      }
      else if(STR == std::string("GAUT"))
      {
         type = GAUT;
         frTS = TimeSystem::GAL;
         toTS = TimeSystem::UTC;
      }
      else if(STR == std::string("SBUT"))
            // TD ??
      {
         type = SBUT;
         frTS = TimeSystem::GPS;
         toTS = TimeSystem::UTC;
      }
      else if(STR == std::string("GLUT"))
      { 
         type = GLUT;
         frTS = TimeSystem::GLO;
         toTS = TimeSystem::UTC;
      }
      else if(STR == std::string("GPGA"))
      { 
         type = GPGA;
         frTS = TimeSystem::GPS;
         toTS = TimeSystem::GAL;
      }
      else if(STR == std::string("GLGP"))
      { 
         type = GLGP;
         frTS = TimeSystem::GLO;
         toTS = TimeSystem::GPS;
      }
      else if(STR == std::string("QZGP"))
      { 
         type = QZGP;
         frTS = TimeSystem::QZS;
         toTS = TimeSystem::GPS;
      }
      else if(STR == std::string("QZUT"))
      { 
         type = QZUT;
         frTS = TimeSystem::QZS;
         toTS = TimeSystem::UTC;
      }
      else if(STR == std::string("BDUT"))
      { 
         type = BDUT;
         frTS = TimeSystem::BDT;
         toTS = TimeSystem::UTC;
      }
      else if(STR == std::string("BDGP"))
      { 
         type = BDGP;
         frTS = TimeSystem::BDT;
         toTS = TimeSystem::GPS;
      }
      else
      {
         Exception e("Unknown TimeSystemCorrection type: " + str);
         GPSTK_THROW(e);
      }
   }


   std::string TimeSystemCorrection ::
   asString() const
   {
      switch(type) {
         case GPUT: return std::string("GPS to UTC");
         case GAUT: return std::string("GAL to UTC");
         case SBUT: return std::string("SBAS to UTC");
         case GLUT: return std::string("GLO to UTC");
         case GPGA: return std::string("GPS to GAL");
         case GLGP: return std::string("GLO to GPS");
         case QZGP: return std::string("QZS to GPS");
         case QZUT: return std::string("QZS to UTC");
         case BDUT: return std::string("BDT to UTC");
         case BDGP: return std::string("BDT to GPS");
         default:   return std::string("ERROR");
      }
   }


   std::string TimeSystemCorrection ::
   asString4() const
   {
      switch(type) {
         case GPUT: return std::string("GPUT");
         case GAUT: return std::string("GAUT");
         case SBUT: return std::string("SBUT");
         case GLUT: return std::string("GLUT");
         case GPGA: return std::string("GPGA");
         case GLGP: return std::string("GLGP");
         case QZGP: return std::string("QZGP");
         case QZUT: return std::string("QZUT");
         case BDUT: return std::string("BDUT");
         case BDGP: return std::string("BDGP");
         default:   return std::string("ERROR");
      }
   }


   void TimeSystemCorrection ::
   dump(std::ostream& s) const
   {
      s << "Time system correction for " << asString4() << ": "
        << asString() << std::scientific << std::setprecision(12);
      switch(type) {
         case GPUT:
            s << ", A0 = " << A0 << ", A1 = " << A1
              << ", RefTime = week/sow " << refWeek << "/" << refSOW;
            break;
         case GAUT:
            s << ", A0 = " << A0 << ", A1 = " << A1
              << ", RefTime = week/sow " << refWeek << "/" << refSOW;
            break;
         case SBUT:
            s << ", A0 = " << A0 << ", A1 = " << A1
              << ", RefTime = week/sow " << refWeek << "/" << refSOW
              << ", provider " << geoProvider << ", UTC ID = " << geoUTCid;
            break;
         case GLUT:
            s << ", -TauC = " << A0
              << ", RefTime = week/sow " << refWeek << "/" << refSOW;
            break;
         case GPGA:
            s << ", A0G = " << A0 << ", A1G = " << A1
              << ", RefTime = week/sow " << refWeek << "/" << refSOW;
            break;
         case GLGP:
            s << ", TauGPS = " << A0 << " sec, RefTime = yr/mon/day "
              << refYr << "/" << refMon << "/" << refDay;
            break;
         case QZGP:
            s << ", A0 = " << A0 << ", A1 = " << A1
              << ", RefTime = week/sow " << refWeek << "/" << refSOW;
            break;
         case QZUT:
            s << ", A0 = " << A0 << ", A1 = " << A1
              << ", RefTime = week/sow " << refWeek << "/" << refSOW;
            break;
         case BDUT:
            s << ", A0 = " << A0 << ", A1 = " << A1
              << ", RefTime = week/sow " << refWeek << "/" << refSOW;
            break;
         case BDGP:
            s << ", A0 = " << A0 << ", A1 = " << A1
              << ", RefTime = week/sow " << refWeek << "/" << refSOW;
            break;
         default:
            break;
      }
         //s << std::endl;
   }


   bool TimeSystemCorrection ::
   isConverterFor(const TimeSystem& ts1, const TimeSystem& ts2) const
   {
      if(ts1 == ts2)
      {
         Exception e("Identical time systems");
         GPSTK_THROW(e);
      }
      if(ts1 == TimeSystem::Unknown || ts2 == TimeSystem::Unknown)
      {
         Exception e("Unknown time systems");
         GPSTK_THROW(e);
      }
      if((ts1 == frTS && ts2 == toTS) || (ts2 == frTS && ts1 == toTS))
      {
         return true;
      }
      return false;
   }


   double TimeSystemCorrection ::
   Correction(const CommonTime& ct) const
   {
      double corr(0.0), dt;
      TimeSystem fromTS(ct.getTimeSystem());
      GPSWeekSecond gpsws;
      CommonTime refTime;
      Exception e("Unable to compute correction - wrong TimeSystem");
      Exception eSBAS("TimeSystemCorrection SBAS <=> UTC has not been implemented");

      switch(type)
      {
         case GPUT:
            if(fromTS != TimeSystem::GPS && fromTS != TimeSystem::UTC)
            {
               GPSTK_THROW(e);
            }

               // dt = fromTime - refTime
            gpsws = GPSWeekSecond(refWeek,refSOW);
            refTime = gpsws.convertToCommonTime();
            refTime.setTimeSystem(fromTS);
            dt = ct - refTime;

            if(fromTS == TimeSystem::GPS)             // GPS => UTC
               corr = -A0-A1*dt;
            else                                      // UTC => GPS
               corr = A0+A1*dt;

            break;

         case GAUT:
            if(fromTS != TimeSystem::GAL && fromTS != TimeSystem::UTC)
            { GPSTK_THROW(e); }

               // dt = fromTime - refTime
            gpsws = GPSWeekSecond(refWeek,refSOW);
            refTime = gpsws.convertToCommonTime();
            refTime.setTimeSystem(fromTS);
            dt = ct - refTime;

            if(fromTS == TimeSystem::GAL)             // GAL => UTC
               corr = A0+A1*dt;
            else                                      // UTC => GAL
               corr = -A0-A1*dt;

            break;

         case SBUT:
            GPSTK_THROW(eSBAS);
            break;

         case GLUT:
            if(fromTS != TimeSystem::GLO && fromTS != TimeSystem::UTC)
            {
               GPSTK_THROW(e);
            }

            if(fromTS == TimeSystem::GLO)             // GLO => UTC
               corr = A0;
            else                                      // UTC => GLO
               corr = -A0;

            break;

         case GPGA:
            if(fromTS != TimeSystem::GPS && fromTS != TimeSystem::GAL)
            {
               GPSTK_THROW(e);
            }

               // dt = fromTime - refTime
            gpsws = GPSWeekSecond(refWeek,refSOW);
            refTime = gpsws.convertToCommonTime();
            refTime.setTimeSystem(fromTS);
            dt = ct - refTime;

            if(fromTS == TimeSystem::GPS)             // GPS => GAL
               corr = A0+A1*dt;
            else                                      // GAL => GPS
               corr = -A0-A1*dt;

            break;

         case GLGP:
            if(fromTS != TimeSystem::GLO && fromTS != TimeSystem::GPS)
            {
               GPSTK_THROW(e);
            }

            if(fromTS == TimeSystem::GLO)             // GLO => GPS
               corr = A0;
            else                                      // GPS => GLO
               corr = -A0;

            break;

         case QZGP:
            if(fromTS != TimeSystem::QZS && fromTS != TimeSystem::GPS)
            {
               GPSTK_THROW(e);
            }

            if(fromTS == TimeSystem::QZS)             // QZS => GPS
               corr = 0.0;    // TD?
            else                                      // GPS => QZS
               corr = 0.0;    // TD?

            break;

         case QZUT:
            if(fromTS != TimeSystem::QZS && fromTS != TimeSystem::UTC)
            {
               GPSTK_THROW(e);
            }

               // dt = fromTime - refTime
            gpsws = GPSWeekSecond(refWeek,refSOW);
            refTime = gpsws.convertToCommonTime();
            refTime.setTimeSystem(fromTS);
            dt = ct - refTime;

            if(fromTS == TimeSystem::QZS)             // QZS => UTC
               corr = A0+A1*dt;
            else                                      // UTC => QZS
               corr = -A0-A1*dt;

            break;

         case BDUT:
            if(fromTS != TimeSystem::BDT && fromTS != TimeSystem::UTC)
            {
               GPSTK_THROW(e);
            }

               // dt = fromTime - refTime
            gpsws = GPSWeekSecond(refWeek,refSOW);
            refTime = gpsws.convertToCommonTime();
            refTime.setTimeSystem(fromTS);
            dt = ct - refTime;

            if(fromTS == TimeSystem::BDT)             // BDT => UTC
               corr = A0+A1*dt;
            else                                      // UTC => BDT
               corr = -A0-A1*dt;

            break;

         case BDGP:
            if(fromTS != TimeSystem::BDT && fromTS != TimeSystem::GPS)
            {
               GPSTK_THROW(e);
            }

               // dt = fromTime - refTime
            gpsws = GPSWeekSecond(refWeek,refSOW);
            refTime = gpsws.convertToCommonTime();
            refTime.setTimeSystem(fromTS);
            dt = ct - refTime;

            if(fromTS == TimeSystem::BDT)             // BDT => GPS
               corr = A0;
            else                                      // GPS => BDT
               corr = -A0;

            break;

         default:
            Exception e("TimeSystemCorrection is not defined.");
            GPSTK_THROW(e);
            break;
      }

      return corr;
   }

} // namespace gpstk
