#pragma ident "$Id: DayTime.cpp 1514 2008-12-12 17:32:16Z raindave $"



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
 * @file DayTime.cpp
 * gpstk::DayTime - encapsulates date and time-of-day in many formats
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>

#include "gpstkplatform.h"
#include "DayTime.hpp"

namespace gpstk
{
   using namespace std;
   using namespace gpstk::StringUtils;

      // ----------- Part  0: month and day labels -------------------
      //
      /// Long month names for converstion from numbers to strings
   static const char *MonthNames[] = { 
      "Error",
      "January","February", "March", "April",
      "May", "June","July", "August",
      "September", "October", "November", "December"
   };
  
      /// Short month names for converstion from numbers to strings
   static const char *MonthAbbrevNames[] = {
      "err", "Jan", "Feb", "Mar", "Apr", "May", "Jun","Jul",
      "Aug", "Sep", "Oct", "Nov", "Dec"
   };
  
      /// Long day of week names for converstion from numbers to strings
   static const char *DayOfWeekNames[] = {
      "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
      "Friday", "Saturday"
   };

      /// Short day of week names for converstion from numbers to strings
   static const char *DayOfWeekAbbrevNames[] = {
      "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
   };

      // ----------- Part  1: exceptions and constants ---------------
      //

      // mSod is sod*FACTOR and mSec is seconds*FACTOR
      // NB FACTOR must be <, and a factor of, 1,000,000
   const long DayTime::FACTOR = 1000;

      // Seconds per half a GPS week.
   const long DayTime::HALFWEEK = 302400;
      // Seconds per whole GPS week.
   const long DayTime::FULLWEEK = 604800;
      // Seconds per day.
   const long DayTime::SEC_DAY = 86400;
      // Milliseconds in a day.
   const long DayTime::MS_PER_DAY = SEC_DAY*1000;

      // Conversion offset, Julian Date to Modified Julian Date.
   const double DayTime::JD_TO_MJD = 2400000.5;
      // 'Julian day' offset from MJD
   const long DayTime::MJD_JDAY = 2400001L;
      // 'Julian day' of GPS epoch (Jan. 1, 1980).
   const long DayTime::GPS_EPOCH_JDAY = 2444245L;
      // Modified Julian Date of GPS epoch (Jan. 1, 1980).
   const long DayTime::GPS_EPOCH_MJD = 44244L;
      // Modified Julian Date of UNIX epoch (Jan. 1, 1970).
   const long DayTime::UNIX_MJD = 40587L;

      // One nanosecond tolerance.
   const double DayTime::ONE_NSEC_TOLERANCE = 1e-9;
      // One microsecond tolerance.
   const double DayTime::ONE_USEC_TOLERANCE = 1e-6;
      // One millisecond tolerance.
   const double DayTime::ONE_MSEC_TOLERANCE = 1e-3;
      // One second tolerance.
   const double DayTime::ONE_SEC_TOLERANCE = 1;
      // One minute tolerance.
   const double DayTime::ONE_MIN_TOLERANCE = 60;
      // One hour tolerance.
   const double DayTime::ONE_HOUR_TOLERANCE = 3600;
   
      // Tolerance for time equality.
#ifdef _WIN32
   double DayTime::DAYTIME_TOLERANCE = ONE_USEC_TOLERANCE;
#else
   double DayTime::DAYTIME_TOLERANCE = ONE_NSEC_TOLERANCE;
#endif

      // 'julian day' of earliest epoch expressible by DayTime; 1/1/4713 B.C.
   const long DayTime::BEGIN_LIMIT_JDAY=0;
      // 'julian day' of latest 'julian day' expressible by DayTime, 
      // 1/1/4713 A.D.
   const long DayTime::END_LIMIT_JDAY=3442448;
      // earliest representable DayTime
   const DayTime DayTime::BEGINNING_OF_TIME =
      DayTime(DayTime::BEGIN_LIMIT_JDAY, 0, 0.0, DayTime::DAYTIME_TOLERANCE);
      // latest representable DayTime
   const DayTime DayTime::END_OF_TIME =
      DayTime(DayTime::END_LIMIT_JDAY, 0, 0.0, DayTime::DAYTIME_TOLERANCE);

      // If true, check the validity of inputs and throw DayTimeException 
      //  on failure.
   bool DayTime::DAYTIME_TEST_VALID = true;

      // ----------- Part  2: member functions: tolerance -------------
      //
   DayTime& DayTime::setTolerance(const double tol)
      throw()
   {
      tolerance = tol;
      return *this;
   }

      // ----------- Part  3: member functions: constructors ----------
      //
      // Default constructor; initializes to current system time.
   DayTime::DayTime()
      throw(DayTime::DayTimeException)
   {
      init();
      setSystemTime();
   }

      // GPS time with full week constructor.
      // @param GPSWeek full week number
      // @param GPSSecond seconds of week.
      // @param f Time frame (see #TimeFrame)
   DayTime::DayTime(short GPSWeek,
                    double GPSSecond,
                    TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      init();
      setGPSfullweek(GPSWeek, GPSSecond, f);
   }

      // GPS time constructor.
      // @param GPSWeek 10 bit week number.
      // @param GPSSecond Seconds of week.
      // @param year Four-digit year.
      // @param f Time frame (see #TimeFrame)
   DayTime::DayTime(short GPSWeek, 
                    double GPSSecond,
                    short ayear, 
                    TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      init();
      setGPS(GPSWeek, GPSSecond, ayear, f);
   }

      // GPS time constructor.
      // @param GPSWeek GPS 10 bit week number.
      // @param zcount Z-count (seconds of week / 1.5)
      // @param year Four-digit year.
      // @param f Time frame (see #TimeFrame)
   DayTime::DayTime(short GPSWeek,
                    long zcount, 
                    short ayear,
                    TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      init();
      setGPS(GPSWeek, zcount, ayear, f);
   }

      // GPS time constructor.
      // @param fullZcount Full z-count (3 MSB unused, mid 10 bits -
      //   week number, 19 LSB "normal" z-count).
      // @param f Time frame (see #TimeFrame)
   DayTime::DayTime(unsigned long fullZcount, 
                    TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      init();
      setGPS(fullZcount, f);
   }

      // GPS Zcount constructor.
      // @param z GPSZcount object to set to
      // @param f Time frame (see #TimeFrame)
   DayTime::DayTime(const GPSZcount& z,
                    TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      init();
      setGPSZcount(z, f);
   }

      // CommonTime constructor.
      // @param c CommonTime object to set to
      // @param f Time frame (see #TimeFrame)
   DayTime::DayTime(const CommonTime& c,
                    TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      init();
      setCommonTime(c, f);
   }   
   
      // Calendar time constructor.
      // @param yy four-digit year.
      // @param mm month of year (1-based).
      // @param dd day of month (1-based).
      // @param hh hour of day.
      // @param min minutes of hour.
      // @param sec seconds of minute.
      // @param f Time frame (see #TimeFrame)
   DayTime::DayTime(short yy, 
                    short mm,
                    short dd,
                    short hh, 
                    short min, 
                    double sec, 
                    TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      init();
      setYMDHMS(yy, mm, dd, hh, min, sec, f);
   }

      // Modified Julian date time constructor.
      // @param julian Modified Julian date.
      // @param f Time frame (see #TimeFrame)
   DayTime::DayTime(long double mjd,
                    TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      init();
      setMJD(mjd, f);
   }

      // Modified Julian date time constructor.
      // @param MJD Modified Julian date as double.
      // @param f Time frame (see #TimeFrame)
   DayTime::DayTime(double mjd, 
                    TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      init();
      setMJD((long double)(mjd), f);
   }

      // Day of year time constructor.
      // @param year Four-digit year.
      // @param DOY Day of year.
      // @param SOD Seconds of day.
      // @param f Time frame (see #TimeFrame)
   DayTime::DayTime(short year, 
                    short doy, 
                    double sod,
                    TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      init();
      setYDoySod(year,doy,sod,f);
   }

      // UNIX time constructor.
      // @param t timeval structure (typically from gettimeofday()).
      // @param f Time frame (see #TimeFrame)
   DayTime::DayTime(const struct timeval& t, 
                    TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      init();
      setUnix(t,f);
      timeFrame = f;
   }

      // ----------- Part  4: member functions: assignment and copy ----------
      //
      // Copy constructor
   DayTime::DayTime(const DayTime &right)
      throw(DayTime::DayTimeException)
   {
      init();
      *this = right;
   }

      // Assignment operator.
   DayTime& DayTime::operator=(const DayTime& right)
      throw()
   {
      jday = right.jday;
      mSod = right.mSod;
      mSec = right.mSec;
      timeFrame  = right.timeFrame;
      tolerance  = right.tolerance;
      return *this;
   }

      // ----------- Part  5: member functions: arithmetic ------------
      //
      // DayTime difference function.
      // @param right DayTime to subtract from this one.
      // @return difference in seconds.
   double DayTime::operator-(const DayTime& right) const
      throw()
   {
      return ( SEC_DAY * double(jday - right.jday)
            + (double(mSod - right.mSod) + mSec - right.mSec) / FACTOR );
   }

      // Add seconds to this time.
      // @param seconds Number of seconds to increase this time by.
      // @return The new time incremented by \c seconds.
   DayTime DayTime::operator+(double seconds) const
      throw()
   {
      return DayTime(*this).addSeconds(seconds);
   }

      // Subtract seconds from this time.
      // @param seconds Number of seconds to decrease this time by.
      // @return The new time decremented by \c seconds.
   DayTime DayTime::operator-(double seconds) const
      throw()
   {
      return DayTime(*this).addSeconds(-seconds);
   }

      // Add seconds to this time.
      // @param seconds Number of seconds to increase this time by.
   DayTime& DayTime::operator+=(double seconds)
      throw(DayTime::DayTimeException)
   {
      addSeconds(seconds);
      return *this;
   }
   
      // Subtract seconds from this time.
      // @param sec Number of seconds to decrease this time by.
   DayTime& DayTime::operator-=(double seconds)
      throw(DayTime::DayTimeException)
   {
      addSeconds(-seconds);
      return *this;
   }

      // Add seconds to this object.
      // @param seconds Number of seconds to add
   DayTime& DayTime::addSeconds(double seconds)
      throw(DayTime::DayTimeException)
   {
      addLongDeltaTime(0, 0, seconds * FACTOR);
      return *this;
   }

      // Add (integer) seconds to this object.
      // @param seconds Number of seconds to add.
   DayTime& DayTime::addSeconds(long seconds)
      throw(DayTime::DayTimeException)
   {
      long ldd, lds ;
      ldd = seconds / SEC_DAY ;        // days
      seconds %= SEC_DAY ;             // seconds-of-day
      lds = seconds * FACTOR ;
      addLongDeltaTime(ldd, lds, 0) ;
      return *this ;
   }

      // Add (integer) milliseconds to this object.
      // @param msec Number of milliseconds to add.
   DayTime& DayTime::addMilliSeconds(long msec)
      throw(DayTime::DayTimeException)
   {
      long ldd, lds ;
      ldd = msec / MS_PER_DAY ;        // days
      msec %= MS_PER_DAY ;             // milliseconds-of-day
      lds = msec * FACTOR / 1000 ;
      addLongDeltaTime(ldd, lds, 0) ;
      return *this;
   }

      // Add (integer) microseconds to this object.
      // @param usec Number of microseconds to add.
   DayTime& DayTime::addMicroSeconds(long usec)
      throw(DayTime::DayTimeException)
   {
         // NB FACTOR must be <, and a factor of, 1000000 :
      long ldd, lds, mult = (1000000 / FACTOR);
      double ds;
      ldd = usec / (1000000 * SEC_DAY) ;        // days
      usec %= (1000000 * SEC_DAY) ;             // usec-of-day
      lds = usec / mult;                        // long sec/FACTOR's
      ds = double(usec % mult) / mult;          // frac sec/FACTOR's
      addLongDeltaTime(ldd, lds, ds);
      return *this;
   }

      // ----------- Part  6: member functions: comparisons ------------
      //
      // Equality operator.
   bool DayTime::operator==(const DayTime &right) const 
      throw()
   {
      // use the smaller of the two tolerances for comparison
      return (ABS(operator-(right)) <=
         ((tolerance > right.tolerance) ? right.tolerance : tolerance));
   }
      // Inequality operator.
   bool DayTime::operator!=(const DayTime &right) const 
      throw()
   {
      return !(operator==(right));
   }
      // Comparison operator (less-than).
   bool DayTime::operator<(const DayTime &right) const 
      throw()
   {
      return (operator-(right) <
            -((tolerance > right.tolerance) ? right.tolerance : tolerance));
   }
      // Comparison operator (greater-than).
   bool DayTime::operator>(const DayTime &right) const 
      throw()
   {
      return (operator-(right) >
            ((tolerance > right.tolerance) ? right.tolerance : tolerance));
   }
      // Comparison operator (less-than or equal-to).
   bool DayTime::operator<=(const DayTime &right) const 
      throw()
   {
      return !(operator>(right));
   }
      // Comparison operator (greater-than or equal-to).
   bool DayTime::operator>=(const DayTime &right) const 
      throw()
   {
      return !(operator<(right));
   }

   // ----------- Part  7: member functions: time frame-----------------------
   //
   DayTime& DayTime::setAllButTimeFrame(const DayTime& right)
      throw(DayTime::DayTimeException)
   {
      TimeFrame t = timeFrame;
      *this = right;
      timeFrame = t;
      return *this;
   }

   // ----------- Part  8: member functions: get -----------------------------
   //
   // 
   // These routines let you retrieve the time stored in this object.
   //
      // Get Julian Date JD
   double DayTime::JD() const throw()
   {
      return (double(jday) + secOfDay() / SEC_DAY - 0.5);
   }

      // Get Modified Julian Date MJD
   double DayTime::MJD() const
      throw()
   {
      return JD() - JD_TO_MJD ;
   }

      // Get year.
   short DayTime::year() const 
      throw()
   {
      int yy, mm, dd;
      convertJDtoCalendar(jday, yy, mm, dd);
      return yy;
   }

      // Get month of year.
   short DayTime::month() const 
      throw()
   {
      int yy, mm, dd;
      convertJDtoCalendar(jday, yy, mm, dd);
      return mm;
   }

      // Get day of month.
   short DayTime::day() const
      throw()
   {
      int yy,mm,dd;
      convertJDtoCalendar(jday, yy, mm, dd);
      return dd;
   }

      // Get day of week
   short DayTime::dayOfWeek() const 
      throw()
   {
      return (((jday % 7) + 1) % 7) ;
   }

      // Get hour of day.
   short DayTime::hour() const
      throw()
   {
      int hh, mm ;
      double sec ; 
      convertSODtoTime(secOfDay(), hh, mm, sec);
      return hh;
   }

      // Get minutes of hour.
   short DayTime::minute() const
      throw()
   {
      int hh, mm ;
      double sec ;
      convertSODtoTime(secOfDay(), hh, mm, sec);
      return mm;
   }

      // Get seconds of minute.
   double DayTime::second() const
      throw()
   {
      int hh, mm ;
      double sec ;
      convertSODtoTime(secOfDay(), hh, mm, sec) ;
      return sec;
   }

      // Get 10-bit GPS week.
   short DayTime::GPS10bitweek() const 
      throw()
   {
      return (GPSfullweek() % 1024); 
   }

      // Get normal (19 bit) zcount.
   long DayTime::GPSzcount() const 
      throw()
   {
      return (long)((GPSsow() / 1.5) + .5);
   }

      // Same as GPSzcount() but without rounding to nearest zcount.
   long DayTime::GPSzcountFloor() const
      throw()
   {
      return (long)(GPSsow() / 1.5);
   }

      // Get seconds of week.
   double DayTime::GPSsow() const
      throw()
   {
      return double(GPSday() * SEC_DAY) + secOfDay() ;
   }

      // Get full (>10 bits) week 
   short DayTime::GPSfullweek() const
      throw()
   {
      return short(double(jday - GPS_EPOCH_JDAY) / 7) ;
   }

      // Get day of year.
   short DayTime::DOY() const 
      throw()
   {
      int yy, mm, dd;
      convertJDtoCalendar(jday, yy, mm, dd);
      return (jday - convertCalendarToJD(yy, 1, 1) + 1) ;
   }

      // Get object time as a modified Julian date.
   long double DayTime::getMJDasLongDouble() const
      throw()
   {
      return ( (long double)(jday)
               + (long double)(secOfDay() / SEC_DAY - 0.5)
               - (long double)(JD_TO_MJD) );
   }

      // Get object time in UNIX timeval structure.
   struct timeval DayTime::unixTime() const
      throw(DayTime::DayTimeException)
   {
      struct timeval t;

      t.tv_sec = mSod / FACTOR 
         + long(jday - MJD_JDAY - UNIX_MJD) * SEC_DAY ;

      t.tv_usec = (mSod % FACTOR) * (1000000 / FACTOR)
         + long(mSec * (1000000 / FACTOR) + 0.5) ;

      if (t.tv_usec >= 1000000) 
      {
         t.tv_usec -= 1000000; 
         ++t.tv_sec; 
      }

      return t;
   }

      // Get time as 32 bit Z count.
      // The 13 MSBs are week modulo 1024, 19 LSBs are seconds of
      // week in Zcounts.
   unsigned long DayTime::fullZcount() const
      throw()
   {
      return ((long(GPS10bitweek()) << 19) + GPSzcount()) ;
   }

      // Same as fullZcount() but without rounding to nearest zcount.
   unsigned long DayTime::fullZcountFloor() const
      throw()
   {
      return ((long(GPS10bitweek()) << 19) + GPSzcountFloor()) ;
   }

   DayTime::operator GPSZcount() const
      throw(DayTime::DayTimeException)
   {
      try
      {
            // account for rounding error in GPSzcount()
         if(GPSzcount() == GPSZcount::ZCOUNT_WEEK)
         {
            return GPSZcount(GPSfullweek() + 1, 0);
         }
         else
         {
            return GPSZcount(GPSfullweek(), GPSzcount());
         }
      }
      catch (gpstk::InvalidParameter& ip)
      {
         DayTime::DayTimeException de(ip);
         GPSTK_THROW(de);
      }
   }

   DayTime::operator CommonTime() const
      throw(DayTime::DayTimeException)
   {
      try
      {
            // Multiply by 1/FACTOR to convert mSec from milliseconds to seconds.
         return CommonTime().setInternal(jday, mSod, mSec / FACTOR);
      }
      catch (gpstk::InvalidParameter& ip)
      {
         DayTime::DayTimeException de(ip);
         GPSTK_THROW(de);
      }
   }

      // ----------- Part  9: member functions: set -------------
      //
      // Set the object's time using calendar (Y/M/D) date and ordinary (H:M:S)
      //  time.
      // @param yy four-digit year.
      // @param month month of year (1-based).
      // @param day day of month (1-based).
      // @param hour hour of day.
      // @param min minutes of hour.
      // @param sec seconds of minute.
      // @param f Time frame (see #TimeFrame)
      // @return a reference to this object.
   DayTime& DayTime::setYMDHMS(short yy,
                               short month, 
                               short day, 
                               short hour,
                               short min, 
                               double sec,
                               TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      setYMD(yy, month, day, f);
      return setHMS(hour, min, sec, f);
   }

      // Set the object time using GPS time. If the GPS week is
      // 10 bit, the full week is determined from the system clock.
      // @param week Week number (can be 10 bit).
      // @param sow Seconds of week.
      // @param f Time frame (see #TimeFrame)
      // @return a reference to this object.
   DayTime& DayTime::setGPS(short week, 
                            double sow, 
                            TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      short fullweek=week;
      if(week < 1024) {
         DayTime ndt;
         ndt.setSystemTime();
         fullweek = 1024*(ndt.GPSfullweek()/1024) + week;
      }
      return setGPSfullweek(fullweek,sow,f);
   }

      // Set the object's time using GPS time. If the GPS week
      // is 10 bit, the full week is determined from the system clock.
      // @param week GPS 10 bit week number.
      // @param zcount Z-count (seconds of week / 1.5)
      // @param f Time frame (see #TimeFrame)
      // @return a reference to this object.
   DayTime& DayTime::setGPS(short week,
                            long zcount, 
                            TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      return setGPS(week, double(zcount) * 1.5, f);
   }

      // Set the object's time using GPS time.
      // @param week GPS 10 bit week number.
      // @param zcount Z-count (seconds of week / 1.5)
      // @param year Four-digit year.
      // @param f Time frame (see #TimeFrame)
      // @return a reference to this object.
   DayTime& DayTime::setGPS(short week,
                            long zcount,
                            short year,
                            TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      return setGPS(week, double(zcount) * 1.5, year, f);
   }

      // Set the object's time using input GPS 10-bit
      // week and seconds of week; determine the number
      // of GPS week rollovers by deducing it from the input year and week.
      // @param week 10 bit week number.
      // @param sow Seconds of week.
      // @param year Four-digit year.
      // @param f Time frame (see #TimeFrame)
      // @return a reference to this object.
   DayTime& DayTime::setGPS(short week, 
                            double sow, 
                            short year,
                            TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      bool valid = true;

      if (DAYTIME_TEST_VALID)
      {
         if ((year < 1980) || (week < 0))
            valid = false;
      }
      
      if (valid)
      {
         DayTime dt;

            // In case the user supplies the full GPS week number.
         week %= 1024;
         
         dt.setYMD(year, 1, 1);
            // z1 is the number of GPS 10-bit-week roll-overs at the beginning
            //  of the year.
         short z1 = dt.GPSfullweek() / 1024;

         dt.setYMD(year, 12, 31);
            // z2 is the number of GPS 10-bit-week roll-overs at the end
            //  of the year.
         short z2 = dt.GPSfullweek() / 1024;

            // GPS 10-bit-week *does not* roll-over during the year.
         if (z1 == z2) 
         {
            return setGPSfullweek(week + z1 * 1024, sow, f);
         }
            // GPS 10-bit-week *does* roll-over during the year.
         else
         {
            if (week <= 512)  // roll-over happened before week ... use z2
            {
               return setGPSfullweek(week + z2 * 1024, sow, f);
            }
            else if (week > 512)  // roll-over happened after week ... use z1
            {
               return setGPSfullweek(week + z1 * 1024, sow, f);
            }
         }
      }

         // If we get here, valid is false.
      if(DAYTIME_TEST_VALID)
      {
         using gpstk::StringUtils::asString ;
         DayTimeException dte("Input inconsistent: year "
                              + asString<int>(year)
                              + " cannot contain 10-bit GPS week "
                              + asString<short>(week));
         GPSTK_THROW(dte);
      }

      return *this;
   }

      // Set the object's time using GPS time. System time
      // is used to determine which 'zone' of 1024 weeks is assigned.
      // @param Zcount Full z-count (3 MSB unused, mid 10 bits -
      //   week number, 19 LSB "normal" z-count).
      // @param f Time frame (see #TimeFrame)
      // @return a reference to this object.
   DayTime& DayTime::setGPS(unsigned long Zcount, 
                            TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      short cweek = (Zcount >> 19) & 0x3FF ;
      long zcount = Zcount & 0x7FFFFL ;
      if(DAYTIME_TEST_VALID) 
      {
         if(cweek < 0 || cweek > 1023 ||    // Is this is a necessary check?
            zcount < 0 || zcount > 403199 ) 
         { 
            using gpstk::StringUtils::asString ;
            DayTimeException dte("Invalid Full GPS Z-count: "
                                 + asString<unsigned long>(Zcount));
            GPSTK_THROW(dte);
         }
      }
      return setGPS(cweek, zcount, f);
   }

      // Set the object's time using GPS time.
      // @param fullweek Full (i.e. >10bits) GPS week number.
      // @param sow Seconds of week.
      // @param f Time frame (see #TimeFrame)
      // @return a reference to this object.
   DayTime& DayTime::setGPSfullweek(short fullweek,
                                    double sow, 
                                    TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      if(DAYTIME_TEST_VALID) 
      {
         if(fullweek < 0 || 
            sow < 0.0 || 
            sow >= double(FULLWEEK))
         {
            using gpstk::StringUtils::asString ;
            DayTimeException dte("Invalid week/seconds-of-week: " 
                                 + asString<short>(fullweek)+ "/" 
                                 + asString(sow));
            GPSTK_THROW(dte);
         }
      }
      jday = GPS_EPOCH_JDAY + 7 * long(fullweek) + long(sow / SEC_DAY);
      double sod = sow - SEC_DAY * long(sow / SEC_DAY);
      return setSecOfDay(sod, f);
   }

      // Set the object's time using the given GPSZcount.
      // System time is used to disambiguate which 1024 week 'zone'
      // is appropriate.
      // @param z the GPSZcount object to set to
      // @param f Time frame (see #TimeFrame)
      // @return a reference to this object.
   DayTime& DayTime::setGPSZcount(const GPSZcount& z,
                                  TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      setGPS(z.getWeek(), z.getZcount(), f) ;
      return *this ;
   }

      // Set the object's time using the give CommonTime.
      // @param c the CommonTime object to set to
      // @param f Time frame (see #TimeFrame))
      // @return a reference to this object.
   DayTime& DayTime::setCommonTime(const CommonTime& c,
                                   TimeFrame f)
      throw()
   {
      c.get(jday, mSod, mSec);
         // Convert mSec from seconds to milliseconds by multiplying by 1000.
      mSec *= FACTOR;
      timeFrame = f;
      return *this;
   }
   
      // Set the object's time using day of year.
      // @param year Four-digit year.
      // @param day_of_year Day of year.
      // @param sec_of_day Seconds of day.
      // @param f Time frame (see #TimeFrame)
      // @return a reference to this object.
   DayTime& DayTime::setYDoySod(short year, 
                                short doy,
                                double sod,
                                TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      setYDoy(year, doy, f);
      return setSecOfDay(sod, f);
   }

      // Set the object's time using Modified Julian date.
      // This just sets the time to the indicated modified Julian date.
      // @param mjd the MJD of the date & time to set
      // @param f Time frame (see #TimeFrame)
      // @return a reference to this object.
   DayTime& DayTime::setMJD(long double mjd,
                            TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      if(DAYTIME_TEST_VALID) 
      {
         if(mjd < (long double)(BEGIN_LIMIT_JDAY-MJD_JDAY) ||
            mjd > (long double)(END_LIMIT_JDAY-MJD_JDAY))
         {
            DayTimeException dte("Invalid MJD: " 
                                 + gpstk::StringUtils::asString(mjd)) ;
            GPSTK_THROW(dte) ;
         }
      }
      jday = long(mjd + 1.0) ;       // int part of JD contributed by MJD
      long double sod = SEC_DAY * (mjd + 1.0 - double(jday)) ;
      jday += MJD_JDAY - 1 ; 
      mSod = long(FACTOR * sod) ;
      mSec = FACTOR * double(sod) - double(mSod) ;
      realignInternals();
      timeFrame = f ;
      return *this ;
   }

      // set using unix timeval struct
   DayTime& DayTime::setUnix(const struct timeval& t,
                             TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      long sec = t.tv_sec ;
      double dt = double(sec) + (t.tv_usec * 1.e-6) ;
      jday = MJD_JDAY + UNIX_MJD + long(dt / SEC_DAY) ;
      sec -= long(dt / SEC_DAY) * SEC_DAY ;
      dt = double(sec) + (t.tv_usec * 1.e-6) ;
      return setSecOfDay(dt);
   }

      // set using ANSI
   DayTime& DayTime::setANSI(const time_t& t, 
                             TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      long double dt = (long double)(t);
      dt /= SEC_DAY;
      dt += UNIX_MJD;
      return setMJD(dt, f);
   }

      // set using system time
   DayTime& DayTime::setSystemTime()
      throw(DayTime::DayTimeException)
   {
#if defined(ANSI_ONLY)
      time_t t;
      time(&t);
      setANSI(t, LocalSystem);
#elif defined(WIN32)
      _timeb t;
      _ftime(&t);
      timeval tv;
      tv.tv_sec = t.time;
      tv.tv_usec = t.millitm*1000;
      setUnix(tv, LocalSystem);
#else
      timeval t;
      gettimeofday(&t, NULL);
      setUnix(t, LocalSystem);
#endif
      return *this;
   }

      // set using local time
   DayTime& DayTime::setLocalTime()
      throw(DayTime::DayTimeException)
   {
      time_t t;
      time(&t);
      struct tm  *ltod;
      ltod = localtime(&t);
      setYMDHMS(1900 + ltod->tm_year, ltod->tm_mon + 1, ltod->tm_mday,
         ltod->tm_hour, ltod->tm_min, ltod->tm_sec);
      return *this;
   }

      // end of set...() that set Day and Time
      // add set...() that set only Day or only Time
      //
      // Set the object using calendar (Y/M/D) date only (time is unchanged)
   DayTime& DayTime::setYMD(int yy, 
                            int mm, 
                            int dd, 
                            TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      long tempDay = convertCalendarToJD(yy, mm, dd);
      if(DAYTIME_TEST_VALID) 
      {
         int y, m, d;
         convertJDtoCalendar(tempDay, y, m, d);
         if(y != yy || m != mm || d != dd) 
         {
            using gpstk::StringUtils::asString ;
            DayTimeException dte("Invalid yy/mm/dd: " + asString<int>(yy) + "/"
                                 + asString<int>(mm) + "/" + asString<int>(dd)
                                 + " != " + asString<int>(y) + "/"
                                 + asString<int>(m) + "/" + asString<int>(d));
            GPSTK_THROW(dte);
         }
      }
      jday = tempDay;
      timeFrame = f;
      return *this;
   }

      // Set the object using ordinary (H:M:S) time only (day is unchanged)
   DayTime& DayTime::setHMS(int hh,
                            int mm,
                            double sec,
                            TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      double sod = convertTimeToSOD(hh, mm, sec);
      if(DAYTIME_TEST_VALID) 
      {
         int h, m;
         double s;
         convertSODtoTime(sod, h, m, s);
         if(h != hh || m != mm || ABS(s - sec) > tolerance) 
         {
            using gpstk::StringUtils::asString ;
            DayTimeException dte("Invalid hh:mm:ss: " + asString<int>(hh)
                                 + ":" + asString<int>(mm) 
                                 + ":" + asString(sec));
            GPSTK_THROW(dte);
         }
      }
      return setSecOfDay(sod,f);
   }

      // Set the object using seconds of day only (day is unchanged)
   DayTime& DayTime::setSecOfDay(double sod, 
                                 TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      if(DAYTIME_TEST_VALID) 
      {
         if(sod < 0.0 || sod >= double(SEC_DAY)) 
         {
            DayTimeException dte("Invalid seconds-of-day: " 
                                 + gpstk::StringUtils::asString(sod));
            GPSTK_THROW(dte);
         }
      }
      mSod = long(FACTOR * sod);
      mSec = FACTOR * sod - double(mSod);
      realignInternals();
      timeFrame = f;
      return *this;
   }

      // Set the object using year and day of year only (time is unchanged)
   DayTime& DayTime::setYDoy(int yy,
                             int doy,
                             TimeFrame f)
      throw(DayTime::DayTimeException)
   {
      jday = convertCalendarToJD(yy, 1, 1) + doy - 1;
      if(DAYTIME_TEST_VALID)
      {
         int y,m,d;
         convertJDtoCalendar(jday, y, m, d);
         if(y != yy) 
         {
            DayTimeException dte("Invalid Year/Day-of-Year: " 
                                 + gpstk::StringUtils::asString(yy)
                                 + "/" + gpstk::StringUtils::asString(doy));
            GPSTK_THROW(dte);
         }
      }
      timeFrame = f;
      return *this;
   }

      // ----------- Part 10: member functions: setToString, printf ---------
      //
   DayTime& DayTime::setToString(const string& str,
                                 const string& fmt)
      throw(DayTime::DayTimeException, DayTime::FormatException,
            StringException)
   {
      try
      {
            // make an object to return
         DayTime toReturn(*this);
         
            // test completeness of specification given by input
         bool hmjd = false, hsow = false, hweek = false, hfullWeek = false,
            hdow = false, hyear = false, hmonth = false, hday= false,
            hzcount = false, hdoy = false, hfullzcount = false,
            hhour = false, hmin = false, hsec = false, hsod = false,
            hunixsec = false, hunixusec = false;

         double imjd, isow;
         short iweek, ifullWeek, idow, imonth, iday;
         long izcount, idoy;
         long ifullzcount;
         
         short iyear, ihour, imin;
         double isec, isod;
         
            // unix time
         long unixsec, unixusec;
         
            // set the year in case it is not in the input -- for week input
         iyear = toReturn.year();

         string f = fmt;
         string s = str;
         
            // parse strings...  As we process each part, it's removed from 
            // both strings so when we reach 0, we're done
         while ( (s.size() > 0) && (f.size() > 0) )
         {
               // remove everything in f and s up to the first % in f
               // (these parts of the strings must be identical or this will 
               // break after it tries to remove it!)
            while ( (s.length() != 0) && (f.length() != 0) && (f[0] != '%') )
            {
                  // remove that character now and other whitespace
               s.erase(0,1);
               f.erase(0,1);
            }
            
               // check just in case we hit the end of either string...
            if ( (s.length() == 0) || (f.length() == 0) )
               break;
            
               // lose the '%' in f...
            f.erase(0,1);
            
            // if the format string is like %03f, get '3' as the field length.
            string::size_type fieldLength = string::npos;
            
            if (!isalpha(f[0]))
            {
               // This is where we have a specified field length so we should
               // not throw away any more characters
               fieldLength = asInt(f);
               
               // remove everything else up to the next character
               // (in "%03f", that would be 'f')
               while ((!f.empty()) && (!isalpha(f[0])))
                  f.erase(0,1);
               if (f.empty())
                  break;
            }
            else
            {
               // finally, get the character that should end this field, if any
               char delimiter = 0;
               if (f.size() > 1)
               {
                  if (f[1] != '%')
                  {
                     delimiter = f[1];
                     
                     stripLeading(s);
                     fieldLength = s.find(delimiter,0);
                  }

                  // if the there is no delimiter character and the next field
                  // is another part of the time to parse, assume the length
                  // of this field is 1
                  else if (fieldLength == string::npos)
                  {
                     fieldLength = 1;
                  }
               }
            }

            // figure out the next string to be removed.  if there is a
            // field length, use that first
            string toBeRemoved = s.substr(0, fieldLength);
            
            // based on char at f[0], convert input to temporary variable
            switch (f[0]) 
            {
               case 'Q':
               {
                  imjd = asDouble(toBeRemoved);
                  hmjd = true;
               }
               break;

               case 'Z':
               {
                  izcount = asInt(toBeRemoved);
                  hzcount = true;
               }
               break;

               case 's':
               {
                  isod = asDouble(toBeRemoved);
                  hsod = true;
               }
               break;

               case 'g':
               {
                  isow = asDouble(toBeRemoved);
                  hsow = true;
               }
               break;

               case 'w':
               {
                  idow = asInt(toBeRemoved);
                  hdow = true;
               }
               break;

               case 'G':
               {
                  iweek = asInt(toBeRemoved);
                  hweek = true;
               }
               break;

               case 'F':
               {
                  ifullWeek = asInt(toBeRemoved);
                  hfullWeek = true;
               }
               break;

               case 'j':
               {
                  idoy = asInt(toBeRemoved);
                  hdoy = true;
               }
               break;

               case 'b':
               case 'B':
               {
                  string thisMonth(toBeRemoved);
                  lowerCase(thisMonth);
                  
                  if (isLike(thisMonth, "jan.*")) imonth = 1;               
                  else if (isLike(thisMonth, "feb.*")) imonth = 2;
                  else if (isLike(thisMonth, "mar.*")) imonth = 3;
                  else if (isLike(thisMonth, "apr.*")) imonth = 4;
                  else if (isLike(thisMonth, "may.*")) imonth = 5;
                  else if (isLike(thisMonth, "jun.*")) imonth = 6;
                  else if (isLike(thisMonth, "jul.*")) imonth = 7;
                  else if (isLike(thisMonth, "aug.*")) imonth = 8;
                  else if (isLike(thisMonth, "sep.*")) imonth = 9;
                  else if (isLike(thisMonth, "oct.*")) imonth = 10;
                  else if (isLike(thisMonth, "nov.*")) imonth = 11;
                  else if (isLike(thisMonth, "dec.*")) imonth = 12;
                  else
                  {
                     FormatException fe("Invalid month entry for setToString");
                     GPSTK_THROW(fe);
                  }
                  hmonth = true;
               }
               break;

               case 'Y':
               {
                  iyear = asInt(toBeRemoved);
                  hyear = true;
               }
               break;

               case 'y':
               {
                  if (fieldLength == 2)
                  {
                     iyear = asInt(toBeRemoved) + 1900;
                     if (iyear < 1980)
                        iyear += 100;
                  }
                  else if (fieldLength == 3)
                  {
                     iyear = asInt(toBeRemoved) + 1000;
                     if (iyear < 1980)
                        iyear += 100;
                  }
                  else
                     iyear = asInt(toBeRemoved);

                  hyear = true;
               }
               break;

               case 'a':
               case 'A':
               {
                  string thisDay = firstWord(toBeRemoved);
                  lowerCase(thisDay);
                  if (isLike(thisDay, "sun.*")) idow = 0;
                  else if (isLike(thisDay, "mon.*")) idow = 1;
                  else if (isLike(thisDay, "tue.*")) idow = 2;
                  else if (isLike(thisDay, "wed.*")) idow = 3;
                  else if (isLike(thisDay, "thu.*")) idow = 4;
                  else if (isLike(thisDay, "fri.*")) idow = 5;
                  else if (isLike(thisDay, "sat.*")) idow = 6;
                  else
                  {
                     FormatException fe("Invalid day of week for setTostring");
                     GPSTK_THROW(fe);
                  }
                  hdow = true;
               }
               break;

               case 'm':
               {
                  imonth = asInt(toBeRemoved);
                  hmonth = true;
               }
               break;

               case 'd':
               {
                  iday = asInt(toBeRemoved);
                  hday = true;
               }
               break;

               case 'H':
               {
                  ihour = asInt(toBeRemoved);
                  hhour = true;
               }
               break;

               case 'M':
               {
                  imin = asInt(toBeRemoved);
                  hmin = true;
               }
               break;

               case 'S':
               {
                  isec = asDouble(toBeRemoved);
                  isec = double(short(isec));
                  hsec = true;
               }
               break;

               case 'f':
               {
                  isec = asDouble(toBeRemoved);
                  hsec = true;
               }
               break;

               case 'U':
               {
                  unixsec = asInt(toBeRemoved);
                  hunixsec = true;
               }
               break;

               case 'u':
               {
                  unixusec = asInt(toBeRemoved);
                  hunixusec = true;
               }
               break;

               case 'C':
               {
                  ifullzcount = asInt(toBeRemoved);
                  hfullzcount = true;
               }

               default:
               {
                     // do nothing
               }
               break;
            }
               // remove the part of s that we processed
            stripLeading(s,toBeRemoved,1);
            
               // remove the character we processed from f
            f.erase(0,1);    
         }
         
         if ( s.length() != 0  ) 
         {
               // throw an error - something didn't get processed in the strings
            FormatException fe(
               "Processing error - parts of strings left unread - " + s);
            GPSTK_THROW(fe);
         }
         
         if (f.length() != 0)
         {
               // throw an error - something didn't get processed in the strings
            FormatException fe(
               "Processing error - parts of strings left unread - " + f);
            GPSTK_THROW(fe);
         }
         
         if (!hmjd && !hsow && !hweek && !hfullWeek && !hdow && !hmonth &&
             !hday&& !hzcount && !hdoy && !hfullzcount && !hhour && !hmin &&
             !hsec && !hsod && !hunixsec && !hunixusec)
         {
            FormatException fe("Incomplete time specification for setToString");
            GPSTK_THROW(fe);
         }

            // GPS time formats
         if (hsow)
            toReturn.setGPSfullweek(0, isow);
         if (hdow && !hsow)
            toReturn.setGPSfullweek(0, idow * double(SEC_DAY));
         if (hzcount)
            toReturn.setGPSfullweek(0, izcount);
         if (hfullzcount)
            toReturn.setGPS(ifullzcount);
         if (hfullWeek)
            toReturn.setGPSfullweek(ifullWeek, toReturn.GPSsow());
         if (hweek)  // iyear has been set in any case
            toReturn.setGPS(iweek, toReturn.GPSsow(), iyear);

            // Calendar time formats
            /*
            *  Due to inconsistency in number of days/month and 
            *  number of days/year, the call to setYMDHMS() needs
            *  to be performed as an atomic call.  Changing the pieces
            *  individually can lead to inconsistencies in the 
            *  transient states.
            */
         if (hyear || hmonth || hday || hhour || hmin || hsec )
         {
            int nyear = toReturn.year();
            int nmonth = toReturn.month();
            int nday = toReturn.day();
            int nhour = toReturn.hour();
            int nmin = toReturn.minute();
            double dsec = toReturn.second();
         
            if (hyear) nyear = iyear;
            if (hmonth) nmonth = imonth;
            if (hday) nday = iday;
            if (hhour) nhour = ihour;
            if (hmin) nmin = imin;
            if (hmin) dsec = isec;
            toReturn.setYMDHMS(nyear, nmonth, nday,
                            nhour, nmin, dsec);
         }
         
            // DOY time formats
         if (hdoy)
            toReturn.setYDoySod(toReturn.year(), idoy, toReturn.DOYsecond());
         if (hsod)
            toReturn.setYDoySod(toReturn.year(), toReturn.DOY(), isod);

            // MJD time format
         if (hmjd)
            toReturn.setMJD(imjd);

            // unix time formats
         if (hunixsec || hunixusec)
         {
            struct timeval tv = toReturn.unixTime();
            if (hunixsec) tv.tv_sec = unixsec;
            if (hunixusec) tv.tv_usec = unixusec;

            toReturn.setUnix(tv);
         }
         
         *this = toReturn;
         return *this;
      }
      catch(gpstk::Exception& exc)
      {
         DayTime::DayTimeException dte(exc);
         dte.addText("Cannot generate time");
         GPSTK_THROW(dte);
      }
      catch(std::exception& exc)
      {
         DayTime::DayTimeException dte(exc.what());
         dte.addText("Cannot generate time");
         GPSTK_THROW(dte);
      }
   }

      // Format this time into a string.
   string DayTime::printf(const char *fmt) const
      throw(gpstk::StringUtils::StringException)
   {
      string rv = fmt;
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*S"), 
                          string("Sd"), (short)second());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?f"),
                          string("ff"), second());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*G"),
                          string("Ghd"), GPS10bitweek());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*F"),
                          string("Fhd"), GPSfullweek());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?g"),
                          string("gf"), GPSsow());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?s"),
                          string("sf"), DOYsecond());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?Q"),
                          string("QLf"), getMJDasLongDouble());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*Y"),
                          string("Yhd"), year());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*y"),
                          string("yhd"), (short)(year() % 100));
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*m"),
                          string("mhd"), month());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*b"),
                          string("bs"), MonthAbbrevNames[month()]);
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*B"),
                          string("Bs"), MonthNames[month()]);
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*d"),
                          string("dhd"), day());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*H"),
                          string("Hhd"), hour());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*M"),
                          string("Mhd"), minute());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*w"),
                          string("whd"), dayOfWeek());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*a"),
                          string("as"), DayOfWeekAbbrevNames[dayOfWeek()]);
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*A"),
                          string("As"), DayOfWeekNames[dayOfWeek()]);
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*z"),
                          string("zd"), GPSzcountFloor());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*Z"),
                          string("Zd"), GPSzcount());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*U"),
                          string("Ud"), unixTime().tv_sec);
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*u"),
                          string("ud"), unixTime().tv_usec);
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*j"),
                          string("jhd"), DOY());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*C"),
                          string("Cd"), fullZcount());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*c"),
                          string("cd"), fullZcountFloor());

      return rv;
   }

      // Format this time into a string.
   std::string DayTime::asString() const
      throw(gpstk::StringUtils::StringException)
   {
      ostringstream o;
      o << *this;
      return o.str();
   }

     // Dump everything possible, using every get() TD: keep?
   void DayTime::dump(std::ostream& s) const
      throw(DayTime::DayTimeException)
   {
      // warning: the internal representation, even for objects that are equal
      // within the tolerance, may be very different

      s << "  internal: jday " << jday << endl;
      s << "  internal: mSod " << mSod << endl;
      s << "  internal: mSec " << fixed << setprecision(15) << mSec << endl;
      s << "  internal: tolerance " << fixed << setprecision(15) << tolerance << endl;
      s << "  double JD(): " << fixed << setprecision(6) << JD() << endl;
      s << "  double MJD(): " << fixed << setprecision(6) << MJD() << endl;
      s << "  short year(): " << year() << endl;
      s << "  short month(): " << month() << endl;
      s << "  short day(): " << day() << endl;
      s << "  short dayOfWeek(): " << dayOfWeek() << endl;
      int yy,mm,dd;
      getYMD(yy, mm, dd);
      s << "  void getYMD(int& yy, int& mm, int& dd): " << yy << " " << mm
        << " " << dd << endl;
      s << "  short hour(): " << hour() << endl;
      s << "  short minute(): " << minute() << endl;
      s << "  double second(): " << fixed << setprecision(6) << second() 
        << endl;
      s << "  double secOfDay(): " << fixed << setprecision(6) << secOfDay() 
        << endl;
      s << "  short GPS10bitweek(): " << GPS10bitweek() << endl;
      s << "  long GPSzcount(): " << GPSzcount() << endl;
      s << "  long GPSzcountFloor(): " << GPSzcountFloor() << endl;
      s << "  double GPSsecond(): " << fixed << setprecision(6) << GPSsecond() 
        << endl;
      s << "  double GPSsow(): " << fixed << setprecision(6) << GPSsow() 
        << endl;
      s << "  short GPSday(): " << GPSday() << endl;
      s << "  short GPSfullweek(): " << GPSfullweek() << endl;
      s << "  short GPSyear(): " << GPSyear() << endl;
      s << "  short DOYyear(): " << DOYyear() << endl;
      s << "  short DOYday(): " << DOYday() << endl;
      s << "  short DOY(): " << DOY() << endl;
      s << "  double DOYsecond(): "<< fixed << setprecision(6) << DOYsecond() 
        << endl;
      s << "  double MJDdate(): " << fixed << setprecision(6) << MJDdate() 
        << endl;
      s << "  long double getMJDasLongDouble(): "
         << fixed << setprecision(6) << getMJDasLongDouble() << endl;
      struct timeval tv=unixTime();
      s << "  struct timeval unixTime(): " << tv.tv_sec << " " << tv.tv_usec 
        << endl;
      s << "  unsigned long fullZcount(): " << fullZcount() << endl;
      s << "  unsigned long fullZcountFloor(): " << fullZcountFloor() << endl;
   }

      // ----------- Part 11: functions: fundamental conversions -----------
      //
      // These two routines convert 'integer JD' and calendar time; they were
      // derived from Sinnott, R. W. "Bits and Bytes" Sky & Telescope Magazine,
      // Vol 82, p. 183, August 1991, and The Astronomical Almanac, published
      // by the U.S. Naval Observatory.
      // NB range of applicability of this routine is from 0JD (4713BC)
      // to approx 3442448JD (4713AD).
   void DayTime::convertJDtoCalendar(long jd, 
                                     int& iyear, 
                                     int& imonth,
                                     int& iday)
      throw()
   {
      long L, M, N, P, Q;
      if(jd > 2299160)    // after Oct 4, 1582
      {
         L = jd + 68569;
         M = (4 * L) / 146097;
         L = L - ((146097 * M + 3) / 4);
         N = (4000 * (L + 1)) / 1461001;
         L = L - ((1461 * N) / 4) + 31;
         P = (80 * L) / 2447;
         iday = int(L - (2447 * P) / 80);
         L = P / 11;
         imonth = int(P + 2 - 12 * L);
         iyear = int(100 * (M - 49) + N + L);
      }
      else 
      {
         P = jd + 1402;
         Q = (P - 1) / 1461;
         L = P - 1461 * Q;
         M = (L - 1) / 365 - L / 1461;
         N = L - 365 * M + 30;
         P = (80 * N) / 2447;
         iday = int(N - (2447 * P) / 80);
         N = P / 11;
         imonth = int(P + 2 - 12 * N);
         iyear = int(4 * Q + M + N - 4716);
         if(iyear <= 0) 
         {
            --iyear;
         }
      }
         // catch century/non-400 non-leap years
      if(iyear > 1599 && 
         !(iyear % 100) && 
         (iyear % 400) && 
         imonth == 2 && 
         iday == 29)
      {
         imonth = 3;
         iday = 1;
      }
   }
   
   long DayTime::convertCalendarToJD(int yy, 
                                     int mm,
                                     int dd) 
      throw()
   {
      if(yy == 0)
         --yy;         // there is no year 0

      if(yy < 0) 
         ++yy;
      
      long jd;
      double y = double(yy), m = double(mm), d = double(dd);

         // In the conversion from the Julian Calendar to the Gregorian
         // Calendar the day after October 4, 1582 was October 15, 1582.
         //
         // if the date is before October 15, 1582
      if(yy < 1582 || (yy == 1582 && (mm < 10 || (mm == 10 && dd < 15))))
      {
         jd = 1729777 + dd + 367 * yy 
            - long(7 * ( y + 5001 + long((m - 9) / 7)) / 4) 
            + long(275 * m / 9);
      }
      else   // after Oct 4, 1582
      {     
        jd = 1721029 + dd + 367 * yy 
           - long(7 * (y + long((m + 9) / 12)) / 4)
           - long(3 * (long((y + (m - 9) / 7) / 100) + 1) / 4) 
           + long(275 * m / 9);

            // catch century/non-400 non-leap years
         if( (! (yy % 100) && 
              (yy % 400) && 
              mm > 2 && 
              mm < 9)      || 
             (!((yy - 1) % 100) &&
              ((yy - 1) % 400) &&
              mm == 1)) 
         {
            --jd;
         }
      }
      return jd;
   }

   void DayTime::convertSODtoTime(double sod, 
                                  int& hh,
                                  int& mm,
                                  double& sec) 
      throw()
   {
         // Get us to within one day.
      if (sod < 0)
      {
         sod += (1 + (unsigned long)(sod / SEC_DAY)) * SEC_DAY ;
      }
      else if (sod >= SEC_DAY)
      {
         sod -= (unsigned long)(sod / SEC_DAY) * SEC_DAY ;
      }
      
      double temp;               // variable to hold the integer part of sod
      sod = modf(sod, &temp);    // sod holds the fraction, temp the integer
      long seconds = long(temp); // get temp into a real integer

      hh = seconds / 3600 ;
      mm = (seconds % 3600) / 60 ;
      sec = double(seconds % 60) + sod ;

   }

   double DayTime::convertTimeToSOD(int hh, 
                                    int mm,
                                    double sec) 
      throw()
   {
      return (sec + 60. * (mm + 60. * hh));
   }

      // ----------- Part 12: private functions and member data -----------
      //
   void DayTime::init()
      throw()
   {
      timeFrame = Unknown;
      tolerance = DAYTIME_TOLERANCE;
      jday = 0;
      mSod = 0;
      mSec = 0.0;
   }

   // ldd is days, lds is milliseconds, and ds is milliseconds
   void DayTime::addLongDeltaTime(long ldd, 
                                  long lds,
                                  double ds)
      throw(DayTime::DayTimeException)
   {
         // Use temp variables so that we don't modify our
         // data members until we know these values are good.
      long workingJday(jday), workingMsod(mSod) ;
      double workingMsec(mSec), temp(0) ;
      
      workingMsec += ds ;
         // check that workingMsod is not out of bounds
      if (workingMsec < 0.)
      {
            // split workingMsec into integer and fraction parts
            // workingMsec gets the fraction and temp gets the integer
         workingMsec = 1 + modf(workingMsec, &temp);
            // add the (negative) integer milliseconds to lds
         if(workingMsec == 1) {
            workingMsec = 0;
            lds += long(temp);
         }
         else
            lds += long(temp) -  1;
      }
      else if (workingMsec >= 1.0)
      {
            // same as above
         workingMsec = modf(workingMsec, &temp) ;
            // add the integer milliseconds to lds
         lds += long(temp) ;         
      }
      
      workingMsod += lds ;
         // add any full days to ldd
      ldd += workingMsod / (SEC_DAY * FACTOR) ;

         // this will get us here:
         // -(SEC_DAY * FACTOR) < workingMsod < (SEC_DAY * FACTOR)
      workingMsod %= (SEC_DAY * FACTOR) ;

         // this will get us here: 0 <= workingMsod < (SEC_DAY * FACTOR)
      if (workingMsod < 0)
      {
         workingMsod += (SEC_DAY * FACTOR) ;
         --ldd ;
      }

      workingJday += ldd ;
         // check that workingJday is not out of bounds
      if(workingJday < BEGIN_LIMIT_JDAY)
      {
         DayTime::DayTimeException dte("DayTime underflow") ;
         GPSTK_THROW(dte) ;
      }
      if(workingJday > END_LIMIT_JDAY)
      {
         DayTime::DayTimeException dte("DayTime overflow") ;
         GPSTK_THROW(dte) ;
      }
      
         // everything's OK, so set the data members
      jday = workingJday ;
      mSod = workingMsod ;
      mSec = workingMsec ;

      realignInternals();
   }

      // Helper routine to realign the internal representation in order to
      // avoid incorrect output (printf) when mSec is within tolerance of 1
   void DayTime::realignInternals(void)
      throw()
   {
      if(fabs(mSec-1)/FACTOR < tolerance) {
         // decrement mSec, except mSec must not be negative
         // alternately, set mSec = 0, but perhaps this contributes numerical noise?
         mSec = (mSec-1 < 0 ? 0 : mSec-1);
         mSod += 1;
      }
      if(mSod >= SEC_DAY*FACTOR) {
         mSod -= SEC_DAY*FACTOR;
         jday += 1;
      }
   }

      // ----------- Part 13: operator<< --------------
      //
      // Stream output for DayTime objects.  Typically used for debugging.
      // @param s stream to append formatted DayTime to.
      // @param t DayTime to append to stream \c s.
      // @return reference to \c s.
   ostream& operator<<( ostream& s, 
                        const DayTime& t )
   {
      s << t.printf("%02m/%02d/%04Y %02H:%02M:%02S");
      return s;
   }

}   // end namespace gpstk

