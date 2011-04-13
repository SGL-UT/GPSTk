#pragma ident "$Id: Epoch.cpp 1163 2008-03-28 13:01:33Z snelsen $"



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
 * @file Epoch.cpp
 * gpstk::Epoch - encapsulates date and time-of-day
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>

#include "gpstkplatform.h"
#include "Epoch.hpp"

#include "TimeConstants.hpp"
#include "TimeString.hpp"

#include "JulianDate.hpp"
#include "MJD.hpp"
#include "YDSTime.hpp"
#include "CivilTime.hpp"
#include "GPSWeekZcount.hpp"
#include "GPSWeekSecond.hpp"


namespace gpstk
{
   using namespace std;
   using namespace gpstk::StringUtils;

      // One nanosecond tolerance.
   const double Epoch::ONE_NSEC_TOLERANCE = 1e-9;
      // One microsecond tolerance.
   const double Epoch::ONE_USEC_TOLERANCE = 1e-6;
      // One millisecond tolerance.
   const double Epoch::ONE_MSEC_TOLERANCE = 1e-3;
      // One second tolerance.
   const double Epoch::ONE_SEC_TOLERANCE = 1;
      // One minute tolerance.
   const double Epoch::ONE_MIN_TOLERANCE = 60;
      // One hour tolerance.
   const double Epoch::ONE_HOUR_TOLERANCE = 3600;
   
      // Tolerance for time equality.
#ifdef _WIN32
   double Epoch::EPOCH_TOLERANCE = ONE_USEC_TOLERANCE;
#else
   double Epoch::EPOCH_TOLERANCE = ONE_NSEC_TOLERANCE;
#endif

      /// Earliest representable Epoch.
   const Epoch BEGINNING_OF_TIME(CommonTime::BEGINNING_OF_TIME);
      /// Latest Representable Epoch.
   const Epoch END_OF_TIME(CommonTime::END_OF_TIME);

   std::string Epoch::PRINT_FORMAT("%02m/%02d/%04Y %02H:%02M:%02S");

   Epoch& Epoch::setTolerance(double tol)
      throw()
   {
      tolerance = tol;
      return *this;
   }
   
      // Default constructor; initializes to current system time.
   Epoch::Epoch(const TimeTag& tt)
      throw(Epoch::EpochException)
         : tolerance(EPOCH_TOLERANCE)
   {
      set(tt);
   }

   Epoch::Epoch(const CommonTime& ct)
      throw()
         : tolerance(EPOCH_TOLERANCE),
           core(ct)
   {}

      /** 
       * TimeTag + Year Constructor.
       * Set the current time using the given year as a hint.
       */
   Epoch::Epoch(const TimeTag& tt,
                short year)
      throw(Epoch::EpochException)
         : tolerance(EPOCH_TOLERANCE)
   {
      set(tt, year);
   }

      // GPS Zcount constructor.
      // @param z GPSZcount object to set to
      // @param f Time frame (see #TimeFrame)
   Epoch::Epoch(const GPSZcount& z)
      throw()
         : tolerance(EPOCH_TOLERANCE)
   {
      set(z);
   }

      // Copy constructor
   Epoch::Epoch(const Epoch& right)
      throw()
         : core(right.core),
           tolerance(right.tolerance)
   {}

      // Assignment operator.
   Epoch& Epoch::operator=(const Epoch& right)
      throw()
   {
      core = right.core;
      tolerance = right.tolerance;
      return *this;
   }

      // Epoch difference function.
      // @param right Epoch to subtract from this one.
      // @return difference in seconds.
   double Epoch::operator-(const Epoch& right) const
      throw()
   {
      return core - right.core;
   }

      // Add seconds to this time.
      // @param seconds Number of seconds to increase this time by.
      // @return The new time incremented by \c seconds.
   Epoch Epoch::operator+(double seconds) const
      throw(Epoch::EpochException)
   {
      return Epoch(*this).addSeconds(seconds);
   }

      // Subtract seconds from this time.
      // @param seconds Number of seconds to decrease this time by.
      // @return The new time decremented by \c seconds.
   Epoch Epoch::operator-(double seconds) const
      throw(Epoch::EpochException)
   {
      return Epoch(*this).addSeconds(-seconds);
   }

      // Add seconds to this time.
      // @param seconds Number of seconds to increase this time by.
   Epoch& Epoch::operator+=(double seconds)
      throw(Epoch::EpochException)
   {
      return addSeconds(seconds);
   }
   
      // Subtract seconds from this time.
      // @param sec Number of seconds to decrease this time by.
   Epoch& Epoch::operator-=(double seconds)
      throw(Epoch::EpochException)
   {
      return addSeconds(-seconds);
   }

      // Add seconds to this object.
      // @param seconds Number of seconds to add
   Epoch& Epoch::addSeconds(double seconds)
      throw(Epoch::EpochException)
   {
      try
      {
         core.addSeconds(seconds);
         return *this;
      }
      catch( InvalidRequest& ir )
      {
         Epoch::EpochException ee(ir);
         GPSTK_THROW(ee);
      }
   }

      // Add (integer) seconds to this object.
      // @param seconds Number of seconds to add.
   Epoch& Epoch::addSeconds(long seconds)
      throw(Epoch::EpochException)
   {
      try
      {
         core.addSeconds(seconds);
         return *this ;
      }
      catch( InvalidRequest& ir )
      {
         Epoch::EpochException ee(ir);
         GPSTK_THROW(ee);
      }
   }

      // Add (integer) milliseconds to this object.
      // @param msec Number of milliseconds to add.
   Epoch& Epoch::addMilliSeconds(long msec)
      throw(Epoch::EpochException)
   {
      try
      {
         core.addMilliseconds(msec);
         return *this;
      }
      catch( InvalidRequest& ir )
      {
         Epoch::EpochException ee(ir);
         GPSTK_THROW(ee);
      }
   }

      // Add (integer) microseconds to this object.
      // @param usec Number of microseconds to add.
   Epoch& Epoch::addMicroSeconds(long usec)
      throw(Epoch::EpochException)
   {
      try
      {
         long ms = usec / 1000;     // whole milliseconds
         usec -= ms * 1000;         // leftover microseconds
         core.addMilliseconds(ms);
            // us * 1ms/1000us * 1s/1000ms
         core.addSeconds(static_cast<double>(usec) * 1e-6);
         return *this;
      }
      catch( InvalidRequest& ir )
      {
         Epoch::EpochException ee(ir);
         GPSTK_THROW(ee);
      }
   }

      // Equality operator.
   bool Epoch::operator==(const Epoch &right) const 
      throw()
   {
      // use the smaller of the two tolerances for comparison
      return (ABS(operator-(right)) <=
         ((tolerance > right.tolerance) ? right.tolerance : tolerance));
   }

      // Inequality operator.
   bool Epoch::operator!=(const Epoch &right) const 
      throw()
   {
      return !(operator==(right));
   }

      // Comparison operator (less-than).
   bool Epoch::operator<(const Epoch &right) const 
      throw()
   {
      return (operator-(right) <
            -((tolerance > right.tolerance) ? right.tolerance : tolerance));
   }

      // Comparison operator (greater-than).
   bool Epoch::operator>(const Epoch &right) const 
      throw()
   {
      return (operator-(right) >
            ((tolerance > right.tolerance) ? right.tolerance : tolerance));
   }
   
      // Comparison operator (less-than or equal-to).
   bool Epoch::operator<=(const Epoch &right) const 
      throw()
   {
      return !(operator>(right));
   }

      // Comparison operator (greater-than or equal-to).
   bool Epoch::operator>=(const Epoch &right) const 
      throw()
   {
      return !(operator<(right));
   }

   template <class TimeTagType>
   TimeTagType Epoch::get() const
      throw(Epoch::EpochException)
   {
      try
      {
         return TimeTagType(core);
      }
      catch( Exception& e)
      {
         EpochException ee(e);
         GPSTK_THROW(ee);
      }
   }
   
      /// Get Julian Date JD
      /// @warning For some compilers, this result may have diminished 
      /// accuracy.
   long double Epoch::JD() const
      throw(Epoch::EpochException)
   {
      return get<JulianDate>().jd;
   }
   
      /// Get Modified Julian Date MJD
      /// @warning For some compilers, this result may have diminished 
      /// accuracy.
   long double Epoch::MJD() const
      throw(Epoch::EpochException)
   {
      return get<gpstk::MJD>().mjd;
   }
   
      /// Get year.
   short Epoch::year() const
      throw(Epoch::EpochException)
   {
      return static_cast<short>(get<CivilTime>().year);
   }
   
      /// Get month of year.
   short Epoch::month() const
      throw(Epoch::EpochException)
   {
      return static_cast<short>(get<CivilTime>().month);
   }
   
      /// Get day of month.
   short Epoch::day() const
      throw(Epoch::EpochException)
   {
      return static_cast<short>(get<CivilTime>().day);
   }
   
      /// Get day of week
   short Epoch::dow() const
      throw(Epoch::EpochException)
   {
      return (((static_cast<long>(JD()) % 7) + 1) % 7) ;
   }
   
      /// Get hour of day.
   short Epoch::hour() const
      throw(Epoch::EpochException)
   {
      return static_cast<short>(get<CivilTime>().hour);
   }
   
      /// Get minutes of hour.
   short Epoch::minute() const
      throw(Epoch::EpochException)
   {
      return static_cast<short>(get<CivilTime>().minute);
   }
   
      /// Get seconds of minute.
   double Epoch::second() const
      throw(Epoch::EpochException)
   {
      return get<CivilTime>().second;
   }
   
      /// Get seconds of day.
   double Epoch::sod() const
      throw(Epoch::EpochException)
   {
      return get<YDSTime>().sod;
   }
   
      /// Get 10-bit GPS week.
   short Epoch::GPSweek10() const
      throw(Epoch::EpochException)
   {
      return static_cast<short>(get<GPSWeekSecond>().getWeek10());
   }
   
      /// Get normal (19 bit) zcount.
   long Epoch::GPSzcount() const
      throw(Epoch::EpochException)
   {
      return get<GPSWeekZcount>().zcount;
   }
   
      /// Same as GPSzcount() but without rounding to nearest zcount.
   long Epoch::GPSzcountFloor() const
      throw(Epoch::EpochException)
   {
      Epoch e = *this + .75; // add half a zcount
      return e.get<GPSWeekZcount>().zcount;
   }
   
      /**
       * Get time as 32 bit Z count.
       * The 13 MSBs are week modulo 1024, 19 LSBs are seconds of
       * week in Zcounts.
       */
   unsigned long Epoch::GPSzcount32() const
      throw(Epoch::EpochException)
   {
      return get<GPSWeekZcount>().getZcount32();
   }
   
      /// Same as fullZcount() but without rounding to nearest zcount.
   unsigned long Epoch::GPSzcount32Floor() const
      throw(Epoch::EpochException)
   {
      Epoch e = *this + .75; // add half a zcount
      return e.get<GPSWeekZcount>().getZcount32();
   }
   
      /// Get GPS second of week.
   double Epoch::GPSsow() const
      throw(Epoch::EpochException)
   {
      return get<GPSWeekSecond>().sow;
   }
   
      /// Get full (>10 bits) week 
   short Epoch::GPSweek() const
      throw(Epoch::EpochException)
   {
      return static_cast<short>(get<GPSWeekSecond>().week);
   }
   
      /// Get day of year.
   short Epoch::doy() const
      throw(Epoch::EpochException)
   {
      return static_cast<short>(get<YDSTime>().doy);
   }
   
      /// Get object time in UNIX timeval structure.
   struct timeval Epoch::unixTime() const
      throw(Epoch::EpochException)
   {
      return get<UnixTime>().tv;
   }
   
   Epoch::operator GPSZcount() const
      throw(Epoch::EpochException)
   {
      try
      {
            // this wants a rounded zcount
         Epoch e = *this + 0.75;
         GPSWeekZcount wz = get<GPSWeekZcount>();
         return GPSZcount(wz.week, wz.zcount);
      }
      catch (gpstk::InvalidParameter& ip)
      {
         Epoch::EpochException ee(ip);
         GPSTK_THROW(ee);
      }
   }

   Epoch::operator CommonTime() const
      throw()
   {
      return core;
   }

   Epoch& Epoch::set(const TimeTag& tt)
      throw(Epoch::EpochException)
   {
      try
      {
         core = tt;
         return *this;
      }
      catch(InvalidParameter& ip)
      {
         EpochException ee(ip);
         GPSTK_THROW(ee);
      }
   }

   Epoch& Epoch::set(const TimeTag& tt,
                     short year)
      throw(Epoch::EpochException)
   {
      try
      { 
         GPSWeekSecond ws(tt);
         ws.setEpoch(whichGPSEpoch(ws.getWeek10(), year));
         core = ws;
         return *this;
      }
      catch(InvalidParameter& ip)
      {
         EpochException ee(ip);
         GPSTK_THROW(ee);
      }
   }

   Epoch& Epoch::set(const CommonTime& c)
      throw()
   {
      core = c;
      return *this;
   }

      // Set the object's time using the given GPSZcount.
      // System time is used to disambiguate which 1024 week 'zone'
      // is appropriate.
      // @param z the GPSZcount object to set to
      // @param f Time frame (see #TimeFrame)
      // @return a reference to this object.
   Epoch& Epoch::set(const GPSZcount& z)
      throw(Epoch::EpochException)
   {
      try
      {
         GPSWeekZcount wz(core);
         wz.week = z.getWeek();
         wz.zcount = z.getZcount();
         core = wz;
         return *this ;
      }
      catch(Exception& exc)
      {
         EpochException ee(exc);
         GPSTK_THROW(ee);
      }
   }

   Epoch& Epoch::setTime(const CommonTime& ct)
      throw(Epoch::EpochException)
   {
      try
      {
         long myDAY, mySOD, ctDAY, ctSOD;
         double myFSOD, ctFSOD;
         core.get(myDAY, mySOD, myFSOD);
         ct.get(ctDAY, ctSOD, ctFSOD);
         core.set(myDAY, ctSOD, ctFSOD);
         return *this;
      }
      catch(InvalidParameter& ip)
      {
         EpochException ee(ip);
         GPSTK_THROW(ee);
      }
   }
   
   Epoch& Epoch::setDate(const CommonTime& ct)
      throw(Epoch::EpochException)
   {
      try
      {
         long myDAY, mySOD, ctDAY, ctSOD;
         double myFSOD, ctFSOD;
         core.get(myDAY, mySOD, myFSOD);
         ct.get(ctDAY, ctSOD, ctFSOD);
         core.set(ctDAY, mySOD, myFSOD);
         return *this;
      }
      catch(InvalidParameter& ip)
      {
         EpochException ee(ip);
         GPSTK_THROW(ee);
      }
   }
   
      // set using local time
   Epoch& Epoch::setLocalTime()
      throw(Epoch::EpochException)
   {
      time_t t;
      time(&t);
      struct tm  *ltod;
      ltod = localtime(&t);
      return set(CivilTime(1900 + ltod->tm_year, 
                           ltod->tm_mon + 1,
                           ltod->tm_mday,
                           ltod->tm_hour,
                           ltod->tm_min,
                           ltod->tm_sec));
   }

   Epoch& Epoch::scanf(const string& str,
                       const string& fmt)
      throw(StringException)
   {
      try
      {
         scanTime( core, str, fmt );
         return *this;
      }
      catch (StringException& se)
      {
         GPSTK_RETHROW(se);
      }
   }

      // Format this time into a string.
   string Epoch::printf(const string& fmt) const
      throw(StringException)
   {
      try
      {
         return printTime( core, fmt );
      }
      catch (StringException& se)
      {
         GPSTK_RETHROW(se);
      }
   }

   short Epoch::whichGPSEpoch(int week,
                              int year) const
      throw(Epoch::EpochException)
   {
      try
      {
            // Get the epoch at the start of the given year.
         short epoch1 = GPSWeekSecond(CivilTime(year, 1, 1)).getEpoch();
            // Get the epoch at the end of the given year.
         short epoch2 = GPSWeekSecond(CivilTime(year, 12, 31)).getEpoch();
         
         if (epoch1 == epoch2)
         {
               // The GPS 10-bit week *doesn't* rollover during the given year.
            return epoch1;
         }

         if (week <= 512)
         {
               // roll-over happened before week -> use epoch2
            return epoch2;
         }
         
            // week > 512
            // roll-over happened after week -> use epoch1
         return epoch1;
      }
      catch( InvalidParameter& ip )
      {
         EpochException ee(ip);
         GPSTK_THROW(ee);
      }
   }

      // Stream output for Epoch objects.  Typically used for debugging.
      // @param s stream to append formatted Epoch to.
      // @param t Epoch to append to stream \c s.
      // @return reference to \c s.
   ostream& operator<<( ostream& s, 
                        const Epoch& e )
   {
      s << e.printf();
      return s;
   }

}   // end namespace gpstk
