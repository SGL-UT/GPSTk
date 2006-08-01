#pragma ident "$Id$"


/**
 * @file DayTime.hpp
 * gpstk::DayTime - encapsulates date and time-of-day in many formats
 */

#ifndef GPSTK_DAYTIME_HPP
#define GPSTK_DAYTIME_HPP

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






#include <string>
#ifdef _MSC_VER
// timeval is defined in winsock.h, which we don't want to include
// because it breaks lots of this code
struct timeval {
  long    tv_sec;         /* seconds */
  long    tv_usec;        /* and microseconds */
};
#else
#include <sys/time.h>
#endif

#include "MathBase.hpp"
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "GPSZcount.hpp"
#include "CommonTime.hpp"

namespace gpstk
{
      /** @defgroup timegroup GPStk Time Group */
      //@{

      /**
       * A time representation class for all common time formats, including
       * GPS. There is a seamless conversion between dates, times, and both,
       * as well as the ability to input and output the stored day-time in
       * formatted strings (printf() and setToString()).
       *
       * Internally, the representation of day and time uses three quantities,
       * (1) jday, an integer representation of Julian Date, specifically
       * jday = int(JD+0.5) or jday=int(MJD+2400001). [Recall that JD = MJD +
       * 2400000.5 and MJD is an integer when second-of-day==0. N.B. jday is
       * NOT == JD or Julian Date, but DayTime::JD() does return JD.]
       * (2) mSod, the integer part of milliseconds of the day, and
       * (3) mSec, the (double) fractional part of milliseconds of the day.
       *
       * In addition, the representation includes a tolerance value (see below)
       * and a time frame. The time frame is a simple way of denoting the
       * origin or type of day-time which is stored in the object. See 
       * #TimeFrame for the list of possible values. The time frame of an 
       * object is determined in the call to a constructor (default is 
       * timeFrame=Unknown), and carried forward into other objects.  It may be
       * read or changed using member functions setAllButTimeFrame(), 
       * setTimeFrame(), and getTimeFrame().
       * 
       * The member datum 'double tolerance' is used in DayTime comparisons.
       * It defaults to the value of the static 
       * gpstk::DayTime::DAYTIME_TOLERANCE, but this can be modified with the 
       * static method setDayTimeTolerance().  Several different default 
       * tolerances have been defined and are in the DayTime-Specific 
       * Definitions section. The tolerance can also be changed on a per object
       * basis with the setTolerance() member function.  All comparisons are 
       * done using the tolerance as a range for the comparison.
       * So, for example, operator==() returns true if the times are within
       * 'tolerance' seconds. Once set for each object, the tolerance is
       * appropriately "carried forward" to new objects through the copy
       * operator (DayTime::operator=), the copy constructor, and elsewhere.
       *
       * The internal representation is manipulated using four fundamental
       * routines, two that convert between 'jday' (the integer
       * representation of JD) and calendar date: year/month/day-of-month,
       * and two that convert between seconds-of-day and hour/minute/second.
       * The range of validity of the jday--calendar routines is approximately
       * 4317 B.C. to 4317 A.D.; these limits are incorporated into constants
       * DayTime::BEGINNING_OF_TIME and DayTime::END_OF_TIME.
       * 
       * * All DayTime objects that lie outside these limits are disallowed. *
       *
       * This internal representation allows close to the maximum precision
       * possible in the time-of-day. Although, note that the code uses 
       * FACTOR=1000 everywhere to compute milliseconds, via e.g. 
       * mSec=seconds/FACTOR, and thus FACTOR could be changed to give a 
       * different precision. (This has not been tested.)
       *
       * This representation separates day and time-of-day cleanly.
       * Because day and time are logically separated, it is possible to use
       * DayTime for day only, or for time only. Thus, for example, one
       * could instantiate a DayTime object and only manipulate the date,
       * without reference to time-of-day; or vice-versa. [However in this
       * regard note that the default constructor for DayTime sets the
       * data, not to zero, but to the current (system) time; because there
       * is no year 0, a DayTime object with all zero data is invalid!]
       *
       * When constructing DayTime objects from GPS time values -- such as
       * GPS week and seconds of weeks, or GPS week and z count -- there 
       * may be ambiguity associated with the GPS week. Many receivers
       * and receiver processing software store the GPS week as it appears
       * in the NAV message, as a 10 bit number. This leads to a 1024 week
       * ambiguity when 10 bit GPS weeks are used to specify a DayTime.
       * In general, DayTime uses the system time to disambiguate which 
       * 1024 week period to use. This is a good assumption except when
       * processing binary data from before GPS week rollover, which
       * occured on August 22, 1999.
       *
       */
   class DayTime
   {
   public:
         // ----------- Part  1: exceptions and constants --------------
         /**
          * @ingroup exceptionclass
          * DayTime basic exception class.
          */
      NEW_EXCEPTION_CLASS(DayTimeException, gpstk::Exception);

         /**
          * @ingroup exceptionclass
          * DayTime formatting ("printing") error exception class.
          */
      NEW_EXCEPTION_CLASS(FormatException, gpstk::Exception);

         /// The various time frames
      enum TimeFrame
      {
         Unknown,    /**< unknown time frame */
         UTC,        /**< Coordinated Universal Time (e.g., from NTP) */
         LocalSystem,/**< time from a local system clock */
         GPS_Tx,     /**< GPS transmit Time (paper clock) (e.g., 15 smooth) */
         GPS_Rx,     /**< GPS receive time (paper clock) */
            // (e.g., rx data if clock bias is applied)
         GPS_SV,     /**< SV time frame (e.g., 211 1.5s/6packs) */
         GPS_Receiver/**< Receiver time (e.g., 30s, raw 1.5s) */
      };

         /**
          * @name DayTime-Specific Definitions
          * All of these tolerances are 1/2 of the tolerance they specify.
          * So one nsec tolerance is actually 1/2 an ns added to the time
          * in units of days.
          */
         //@{

         /// time-of-day is stored as long (seconds-of-day)*FACTOR
         /// plus double (remaining seconds)/FACTOR
      static const long FACTOR;

         /// Conversion offset, Julian Date to Modified Julian Date.
      static const double JD_TO_MJD;
         /// 'Julian day' offset from MJD
      static const long MJD_JDAY;
         /// 'Julian day' of GPS epoch (Jan. 1, 1980).
      static const long GPS_EPOCH_JDAY;
         /// Modified Julian Date of GPS epoch (Jan. 1, 1980).
      static const long GPS_EPOCH_MJD;
         /// Modified Julian Date of UNIX epoch (Jan. 1, 1970).
      static const long UNIX_MJD;
         /// Seconds per half a GPS week.
      static const long HALFWEEK;
         /// Seconds per whole GPS week.
      static const long FULLWEEK;
         /// Seconds per day.
      static const long SEC_DAY;
         /// Milliseconds in a day.
      static const long MS_PER_DAY;

         /// One nanosecond tolerance.
      static const double ONE_NSEC_TOLERANCE;
         /// One microsecond tolerance.
      static const double ONE_USEC_TOLERANCE;
         /// One millisecond tolerance.
      static const double ONE_MSEC_TOLERANCE;
         /// One second tolerance.
      static const double ONE_SEC_TOLERANCE;
         /// One minute tolerance.
      static const double ONE_MIN_TOLERANCE;
         /// One hour tolerance.
      static const double ONE_HOUR_TOLERANCE;
      
         /// Default tolerance for time equality in days.
      static double DAYTIME_TOLERANCE;

         /// 'julian day' of earliest epoch expressible by DayTime:
         /// 1/1/4713 B.C.
      static const long BEGIN_LIMIT_JDAY;
         /// 'julian day' of latest epoch expressible by DayTime:
         /// 1/1/4713 A.D.
      static const long END_LIMIT_JDAY;
         /// earliest representable DayTime
      static const DayTime BEGINNING_OF_TIME;
         /// latest representable DayTime
      static const DayTime END_OF_TIME;

         /// If true, check the validity of inputs. 
         /// Throw DayTimeException on failure.
      static bool DAYTIME_TEST_VALID;

         //@}
      
         // ----------- Part  2: member functions: tolerance ------------
         //
         /// Changes the DAYTIME_TOLERANCE for all DayTime objects
      static double setDayTimeTolerance(const double tol)
         throw()
      { DAYTIME_TOLERANCE = tol; return DAYTIME_TOLERANCE; }

         /// Returns the current DAYTIME_TOLERANCE.
      static double getDayTimeTolerance() 
         throw()
      { return DAYTIME_TOLERANCE; }
   
         /**
          * Sets the tolerance for output and comparisons on this object only.
          * See the constants in this file (e.g. ONE_NSEC_TOLERANCE)
          * for some easy to use tolerance values.
          * @param tol Tolerance in days to be used by comparison operators.
          * @sa DayTime-Specific Definitions
          */
      DayTime& setTolerance(const double tol) throw();

         /** 
          * Return the tolerance value currently in use by this object.
          * @return the current tolerance value (in seconds, of course)
          */
      double getTolerance() throw()
      { return tolerance; }


         // ----------- Part  3: member functions: constructors ------------
         //
         /**
          * Default constructor.
          * Initializes to current system time.
          */
      DayTime()
         throw(DayTimeException);

         /**
          * GPS time with full week constructor.
          * @param GPSWeek full week number
          * @param GPSSecond seconds of week.
          * @param f Time frame (see #TimeFrame)
          */
      DayTime(short GPSWeek,
              double GPSSecond,
              TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * GPS time constructor. In the case of 10-bit week input,
          * the year and week are used to deduce the number of GPS
          * week rollovers and thus the full GPS week.
          * @param GPSWeek week number.
          * @param GPSSecond Seconds of week.
          * @param year Four-digit year consistent with GPS input.
          * @param f Time frame (see #TimeFrame)
          */
      DayTime(short GPSWeek,
              double GPSSecond,
              short year, 
              TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * GPS time constructor. In the case of 10-bit week input,
          * the year and week are used to deduce the number of GPS
          * week rollovers and thus the full GPS week.
          * @param GPSWeek GPS week number.
          * @param zcount Z-count (seconds of week / 1.5)
          * @param year Four-digit year consistent with GPS input.
          * @param f Time frame (see #TimeFrame)
          */
      DayTime(short GPSWeek, 
              long zcount,
              short year,
              TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * GPS time constructor given the full Z count.
          * @warn The number of GPS week rollovers, and therefore the
          * full GPS week, is determined from the current system time.
          * @param fullZcount Full z-count (3 MSB unused, mid 10 bits -
          *   week number, 19 LSB "normal" z-count).
          * @param f Time frame (see #TimeFrame)
          */
      DayTime(unsigned long fullZcount, 
              TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * GPS Zcount constructor.
          * @warn The number of GPS week rollovers, and therefore the
          * full GPS week, is determined from the current system time.
          * @param z GPSZcount object to set to
          * @param f Time frame (see #TimeFrame)
          */
      DayTime(const GPSZcount& z,
              TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * CommonTime constructor.
          * @param c CommonTime object to set to
          * @param f Time frame (see #TimeFrame)
          */
      DayTime(const CommonTime& c,
              TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Calendar time constructor.
          * @param year four-digit year.
          * @param month month of year (1-based).
          * @param day day of month (1-based).
          * @param hour hour of day.
          * @param minute minutes of hour.
          * @param second seconds of minute.
          * @param f Time frame (see #TimeFrame)
          */
      DayTime(short year,
              short month,
              short day,
              short hour,
              short minute,
              double second,
              TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Modified Julian date time constructor.
          * @warn For some compilers, this result may have diminished accuracy.
          * @param MJD Modified Julian date as long double.
          * @param f Time frame (see #TimeFrame)
          */
      DayTime(long double MJD, 
              TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Modified Julian date time constructor.
          * @warn For some compilers, this result may have diminished accuracy.
          * @param MJD Modified Julian date as double.
          * @param f Time frame (see #TimeFrame)
          */
      DayTime(double MJD, 
              TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Day of year time constructor.
          * @param year Four-digit year.
          * @param DOY Day of year.
          * @param SOD Seconds of day.
          * @param f Time frame (see #TimeFrame)
          */
      DayTime(short year,
              short doy,
              double sod, 
              TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * UNIX time constructor.
          * @param t timeval structure (typically from gettimeofday()).
          * @param f Time frame (see #TimeFrame)
          */
      DayTime(const struct timeval& t, 
              TimeFrame f = Unknown)
         throw(DayTimeException);

         /// Destructor.
      ~DayTime()
         throw()
      {}

         // --------- Part  4: member functions: assignment and copy ----------
         //
         /// Copy constructor.
      DayTime(const DayTime &right)
         throw(DayTimeException);

         /// Assignment operator.
      DayTime& operator=(const DayTime& right)
         throw();

         // ----------- Part  5: member functions: arithmetic ------------
         //
         /**
          * DayTime difference function.
          * @param right DayTime to subtract from this one.
          * @return difference in seconds.
          */
      double operator-(const DayTime& right) const
         throw();

         /**
          * Add seconds to this time.
          * @param sec Number of seconds to increase this time by.
          * @return The new time incremented by \c sec.
          */
      DayTime operator+(double sec) const
         throw();

         /**
          * Subtract seconds from this time.
          * @param sec Number of seconds to decrease this time by.
          * @return The new time decremented by \c sec.
          */
      DayTime operator-(double sec) const
         throw();

         /**
          * Add seconds to this time.
          * @param sec Number of seconds to increase this time by.
          * @throws DayTimeException on over/under-flow
          */
      DayTime& operator+=(double sec)
         throw(DayTimeException);

         /**
          * Subtract seconds from this time.
          * @param sec Number of seconds to decrease this time by.
          * @throws DayTimeException on over/under-flow
          */
      DayTime& operator-=(double sec)
         throw(DayTimeException);

         /**
          * Add (double) seconds to this time.
          * @param seconds Number of seconds to increase this time by.
          * @throws DayTimeException on over/under-flow
          */
      DayTime& addSeconds(double seconds)
         throw(DayTimeException);

         /**
          * Add (integer) seconds to this time.
          * @param seconds Number of seconds to increase this time by.
          * @throws DayTimeException on over/under-flow
          */
      DayTime& addSeconds(long seconds)
         throw(DayTimeException);

         /**
          * Add (integer) milliseconds to this time.
          * @param msec Number of milliseconds to increase this time by.
          * @throws DayTimeException on over/under-flow
          */
      DayTime& addMilliSeconds(long msec)
         throw(DayTimeException);

         /**
          * Add (integer) microseconds to this time.
          * @param usec Number of microseconds to increase this time by.
          * @throws DayTimeException on over/under-flow
          */
      DayTime& addMicroSeconds(long usec)
         throw(DayTimeException);

         // ----------- Part  6: member functions: comparisons ------------
         //
         /// Equality operator.
         /// @return true if ABS(*this - right) <= lesser of tolerance 
         ///  and right.tolerance; false otherwise
      bool operator==(const DayTime &right) const
         throw();
         /// Inequality operator.
      bool operator!=(const DayTime &right) const
         throw();
         /// Comparison operator (less-than).
      bool operator<(const DayTime &right) const
         throw();
         /// Comparison operator (greater-than).
      bool operator>(const DayTime &right) const
         throw();
         /// Comparison operator (less-than or equal-to).
      bool operator<=(const DayTime &right) const
         throw();
         /// Comparison operator (greater-than or equal-to).
      bool operator>=(const DayTime &right) const
         throw();

         // ----------- Part  7: member functions: time frame ------------
         //
         /**
          * Change time frames via pseudo-copy method.
          * Copies all of \c right except the time frame,
          * which remains unchanged.
          * @param right DayTime object to copy time from.
          * @throws DayTimeException
          */
      DayTime& setAllButTimeFrame(const DayTime& right) 
         throw(DayTimeException);
      
         /// Set the time frame for this time.
      DayTime& setTimeFrame(TimeFrame f)
         throw()
      { timeFrame = f ; return *this ; }
      
         /// Get the time frame for this time.
      TimeFrame getTimeFrame() const 
         throw() 
      { return timeFrame ; }

         // ----------- Part  8: member functions: get --------------
         // 
         // These routines retrieve elements of day, time or both.
         //
         /// Get Julian Date JD
      double JD() const
         throw();

         /// Get Modified Julian Date MJD
         /// @warning For some compilers, this result may have diminished 
         /// accuracy.
      double MJD() const
         throw();

         /// Get year.
      short year() const
         throw();

         /// Get month of year.
      short month() const
         throw();

         /// Get day of month.
      short day() const
         throw();

         /// Get day of week
      short dayOfWeek() const
         throw();

         /// Get year, month and day of month
      void getYMD(int& yy, int& mm, int& dd) const 
         throw() 
      { convertJDtoCalendar(jday, yy, mm, dd) ; }

         /// Get hour of day.
      short hour() const
         throw();

         /// Get minutes of hour.
      short minute() const
         throw();

         /// Get seconds of minute.
      double second() const
         throw();

         /// Get seconds of day.
      double secOfDay() const
         throw() 
      { return (double(mSod) + mSec) / FACTOR ; }

         /// Get 10-bit GPS week.
      short GPS10bitweek() const
         throw();

         /// Get normal (19 bit) zcount.
      long GPSzcount() const
         throw();

         /// Same as GPSzcount() but without rounding to nearest zcount.
      long GPSzcountFloor() const
         throw();

         /// Get seconds of week.
      double GPSsecond() const
         throw() 
      { return GPSsow(); }

         /// Get GPS second of week.
      double GPSsow() const
         throw();

         /// Get day of week.
      short GPSday() const
         throw() 
      { return dayOfWeek(); }

         /// Get full (>10 bits) week 
      short GPSfullweek() const
         throw();

         /// Get year.
      short GPSyear() const
         throw() 
      { return year(); }

         /// Get year. 
      short DOYyear() const
         throw() 
      { return year(); }

         /// Get day of year.
      short DOYday() const
         throw() 
      { return DOY(); }

         /// Get day of year.
      short DOY() const
         throw();

         /// Get seconds of day.
      double DOYsecond() const
         throw() 
      { return secOfDay(); }

         /// Get object time as a modified Julian date.
         /// @warning For some compilers, this result may have diminished 
         ///  accuracy.
      double MJDdate() const
         throw() 
      { return double(getMJDasLongDouble()) ; }

         /// Get object time as a (long double) modified Julian date.
         /// @Warning For some compilers, this result may have diminished
         ///  accuracy.
      long double getMJDasLongDouble() const
         throw();

         /// Get object time in UNIX timeval structure.
      struct timeval unixTime() const
         throw(DayTimeException);

         /**
          * Get time as 32 bit Z count.
          * The 13 MSBs are week modulo 1024, 19 LSBs are seconds of
          * week in Zcounts.
          */
      unsigned long fullZcount() const
         throw();

         /// Same as fullZcount() but without rounding to nearest zcount.
      unsigned long fullZcountFloor() const
         throw();

         /// Convert this object to a GPSZcount object.
         /// @throws DayTimeException if this DayTime's current state
         ///  doesn't constitute a valid GPS Z-count.
      operator GPSZcount() const
         throw(DayTimeException) ;

         /// Convert this object to a CommonTime object.
         /// @throws DayTimeException if this DayTime's current state
         ///  doesn't constitute a valid CommonTime.
      operator CommonTime() const
         throw(DayTimeException);

         // ----------- Part  9: member functions: set ------------
         //
         /**
          * Set the object's time using calendar (Y/M/D) date
          *   and ordinary (H:M:S) time
          * @param year four-digit year.
          * @param month month of year (1-based).
          * @param day day of month (1-based).
          * @param hour hour of day.
          * @param min minutes of hour.
          * @param sec seconds of minute.
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setYMDHMS(short year,
                         short month,
                         short day, 
                         short hour, 
                         short min,
                         double sec, 
                         TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Set the object's time using GPS time.
          * @warn In the case of 10-bit week input, the number of
          * GPS week rollovers, and therefore the full GPS week,
          * is determined from the current system time.
          * @param week GPS week number.
          * @param sow GPS seconds of week.
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setGPS(short week,
                      double sow, 
                      TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Set the object's time using GPS time (week and Z count).
          * @warn In the case of 10-bit week input, the number of
          * GPS week rollovers, and therefore the full GPS week,
          * is determined from the current system time; prefer setGPSfullweek().
          * @param week GPS week number.
          * @param zcount Z-count (seconds of week / 1.5)
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setGPS(short week, 
                      long zcount, 
                      TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Set the object's time using GPS time. In case of 10-bit
          * week input, the year and week are used to deduce the number
          * of GPS week rollovers and thus the full GPS week;
          * prefer setGPSfullweek().
          * @param week GPS week number.
          * @param zcount GPS Z-count (seconds of week / 1.5)
          * @param year Four-digit year consistent with GPS input.
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setGPS(short week, 
                      long zcount,
                      short year, 
                      TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Set the object's time using GPS time. In case of 10-bit
          * week input, the year and week are used to deduce the number
          * of GPS week rollovers and thus the full GPS week;
          * prefer setGPSfullweek().
          * @param week GPS week number.
          * @param sow GPS seconds of week.
          * @param year Four-digit year consistent with GPS input.
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setGPS(short week, 
                      double sow,
                      short year, 
                      TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Set the object's time using GPS time (full Z count).
          * @warn The number of GPS week rollovers, and therefore the
          * full GPS week, is determined from the current system time.
          * @param Zcount Full z-count (3 MSB unused, mid 10 bits -
          *   week number, 19 LSB "normal" z-count).
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setGPS(unsigned long Zcount, 
                      TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Set the object's time using GPS time.
          * @param fullweek Full (i.e. >10bits) GPS week number.
          * @param sow Seconds of week.
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setGPSfullweek(short fullweek,
                              double sow,
                              TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Set the object's time using GPS time.
          * @param fullweek Full (i.e. >10bits) GPS week number.
          * @param zcount Z-count (seconds of week / 1.5)
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setGPSfullweek(short fullweek,
                              long zcount, 
                              TimeFrame f = Unknown)
         throw(DayTimeException)
      { return setGPSfullweek(fullweek, double(zcount) * 1.5, f) ; }

         /**
          * Set the object's time using the given GPSZcount.
          * @warn The number of GPS week rollovers, and therefore the
          * full GPS week, is determined from the current system time.
          * @param z the GPSZcount object to set to
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setGPSZcount(const GPSZcount& z,
                            TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Set the object's time using the give CommonTime.
          * @param c the CommonTime object to set to
          * @param f Time frame (see #TimeFrame))
          * @return a reference to this object.
          */
      DayTime& setCommonTime(const CommonTime& c,
                             TimeFrame f = Unknown)
         throw();

         /**
          * Set the object's time using day of year.
          * @param year Four-digit year.
          * @param day_of_year Day of year.
          * @param sec_of_day Seconds of day.
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setYDoySod(short year,
                          short day_of_year, 
                          double sec_of_day, 
                          TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Set the object's time using (long double) Modified Julian date.
          * This just sets the time to the indicated modified Julian date.
          * @param mjd date/time to set, in MJD.
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setMJD(long double mjd,
                      TimeFrame f = Unknown)
         throw(DayTimeException);

         /// Same as setMJD
      DayTime& setMJDdate(long double mjd, 
                          TimeFrame f = Unknown)
         throw(DayTimeException) 
      { return setMJD(mjd, f); }

         /// Same as setMJD but with input of type double
      DayTime& setMJD(double mjd, 
                      TimeFrame f = Unknown)
         throw(DayTimeException)
      { return setMJD((long double)(mjd), f) ; }

         /// Same as setMJD but with input of type double
      DayTime& setMJDdate(double mjd, 
                          TimeFrame f = Unknown)
         throw(DayTimeException)
      { return setMJD((long double)(mjd), f) ; }

         /**
          * Set the object's time using POSIX structures.
          * This uses the POSIX timeval struct to set the time.  This
          * structure represents the time with a resolution of
          * microseconds.
          * @param t time to set
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setUnix(const struct timeval& t, 
                       TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Set the object's time using ANSI structures.  This uses
          * the ANSI C/C++ time_t struct to set the time.  This
          * structure represents the time with a resolution of
          * seconds.
          * @param t time to set
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setANSI(const time_t& t,
                       TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Set the object's time to the current system time.
          * If the POSIX gettimeofday() system call is available, it
          * is used to determine system time.  If the gettimeofday()
          * call is not available, the ANSI time() system call is used
          * to determine system time.
          * @return a reference to this object.
          */
      DayTime& setSystemTime()
         throw(DayTimeException);

         /**
          * Set the object time to the current local time.
          */
      DayTime& setLocalTime()
         throw(DayTimeException);

         /**
          * Set the object using calendar (Y/M/D) date only (time is unchanged)
          * @param yy four-digit year.
          * @param mm month of year (1-based).
          * @param dd day of month (1-based).
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setYMD(int yy,
                      int mm,
                      int dd,
                      TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Set the object using ordinary (H:M:S) time only (day is unchanged)
          * @param hh hour of day.
          * @param mm minutes of hour.
          * @param sec seconds of minute.
          * @param f Time frame (see #TimeFrame)
          * @return a reference to this object.
          */
      DayTime& setHMS(int hh,
                      int mm,
                      double sec, 
                      TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Set the object using seconds of day only (day is unchanged)
          * @param sod seconds of day.
          */
      DayTime& setSecOfDay(double sod,
                           TimeFrame f = Unknown)
         throw(DayTimeException);

         /**
          * Set the object using year and day of year only (time is unchanged)
          * @param sod seconds of day.
          */
      DayTime& setYDoy(int yy,
                       int doy,
                       TimeFrame f = Unknown)
         throw(DayTimeException);

         // ----------- Part 10: member functions: setToString, printf --------
         //
         // FIX someone figure out how to make the table below show up
         // nice in doxygen.
         /**
          * Similar to scanf, this function takes a string and a
          * format describing string in order to read in daytime
          * values.  The parameters it can take are listed below and
          * described above with the printf() function.
          *
          * The specification must resolve to a day at a minimum
          * level. The following table lists combinations that give valid
          * times. Anything more or other combinations will give
          * unknown (read as: "bad") results so don't try it.  Anything
          * less will throw an exception.  If nothing changes the time
          * of day, it will default to midnight.  Also, the year
          * defaults to the current year if a year isn't specified
          * or can't be determined.
          *
          * @code
          *  1 of...           and 1 of....         optional...
          *  %C
          *  %G                %w %g %Z                %Y %y
          *  %F                %w %g %Z
          *  %m %B %b          %a %A %d             %Y %y %H %M %S
          *  %Q
          *  %j                                      %Y %y %s
          * @endcode
          *
          * So 
          * @code
          * time.setToString("Aug 1, 2000 20:20:20", "%b %d, %Y %H:%M:%S")
          * @endcode
          * works but 
          * @code
          * time.setToString("Aug 2000", "%b %Y")
          * @endcode
          * doesn't work (incomplete specification because it doesn't specify
          * a day).
          *
          * Don't worry about counting whitespace - this function will
          * take care of that.  Just make sure that the extra stuff in
          * the format string (ie '.' ',') are in the same relative
          * location as they are in the actual string.  (see in the
          * example above))
          *
          * @param str string to get date/time from.
          * @param fmt format to use to parse \c str.
          * @throw DayTimeException if \c fmt is an incomplete specification
          * @throw FormatException if unable to scan \c str.
          * @throw StringException if an error occurs manipulating the
          * \c str or \c fmt strings.
          * @return a reference to this object.
          */
      DayTime& setToString(const std::string& str, 
                           const std::string& fmt)
         throw(DayTimeException, FormatException,
               gpstk::StringUtils::StringException);


         // if you can see this, ignore the \'s below, as they are for
         // the nasty html-ifying of doxygen.  Browsers try and
         // interpret the % and they get all messed up.
         /**
          * Format this time into a string.
          *
          * Generate and return a string containing a formatted
          * date, formatted by the specification \c fmt.
          *
          * \li \%Y   year()
          * \li \%y   year() % 100
          * \li \%m   month()
          * \li \%d   day()
          * \li \%H   hour()
          * \li \%M   minute()
          * \li \%S   (short)second()
          * \li \%f   second()
          * \li \%G   GPS10bitweek()
          * \li \%F   GPSfullweek()
          * \li \%g   GPSsecond() or GPSsow()
          * \li \%s   DOYsecond()
          * \li \%Q   MJDdate()
          * \li \%w   dayOfWeek() or GPSday()
          * \li \%b   MonthAbbrevName[month()]
          * \li \%B   MonthName[month()]
          * \li \%a   DayOfWeekAbbrevName[dayOfWeek()]
          * \li \%A   DayOfWeekName[dayOfWeek()]
          * \li \%j   DOYday() or DOY()
          * \li \%Z   GPSzcount()
          * \li \%z   GPSzcountFloor()
          * \li \%U   unixTime().tv_sec
          * \li \%u   unixTime().tv_usec
          * \li \%C   fullZcount()
          * \li \%c   fullZcountFloor()
          *
          * @param fmt format to use for this time.
          * @return a string containing this time in the
          * representation specified by \c fmt.
          */
      std::string printf(const char *fmt) const
         throw(gpstk::StringUtils::StringException);

         /// Format this time into a string. @see printf(const char*)
      std::string printf(const std::string& fmt) const
         throw(gpstk::StringUtils::StringException) 
      { return printf(fmt.c_str()) ; }

         /// Returns the string that operator<<() would print.
      std::string asString() const
         throw(gpstk::StringUtils::StringException);

         /// Dump everything possible, using every get() TD: keep?
      void dump(std::ostream& s) const
         throw(DayTimeException);

         // ----------- Part 11: functions: fundamental conversions ----------
         // 
         /** Fundamental routine to convert from "Julian day" (= JD + 0.5) 
          *  to calendar day.
          * @param jd long integer "Julian day" = JD+0.5
          * @param iyear reference to integer year
          * @param imonth reference to integer month (January == 1)
          * @param iday reference to integer day of month 
          *  (1st day of month == 1)
          * @note range of applicability of this routine is from 0JD (4713BC)
          *  to approx 3442448JD (4713AD).
          * Algorithm references: Sinnott, R. W. "Bits and Bytes,"
          *  Sky & Telescope Magazine, Vol 82, p. 183, August 1991, and
          *  The Astronomical Almanac, published by the U.S. Naval Observatory.
          */
      static void convertJDtoCalendar(long jd, 
                                      int& iyear, 
                                      int& imonth, 
                                      int& iday)
         throw();

         /** Fundamental routine to convert from calendar day to "Julian day"
          *  (= JD + 0.5)
          * @param iyear reference to integer year
          * @param imonth reference to integer month (January == 1)
          * @param iday reference to integer day of month 
          *  (1st day of month == 1)
          * @return jd long integer "Julian day" = JD+0.5
          * @note range of applicability of this routine is from 0JD (4713BC)
          * to approx 3442448JD (4713AD).
          * Algorithm references: Sinnott, R. W. "Bits and Bytes,"
          *  Sky & Telescope Magazine, Vol 82, p. 183, August 1991, and
          *  The Astronomical Almanac, published by the U.S. Naval Observatory.
          */
      static long convertCalendarToJD(int iyear, 
                                      int imonth,
                                      int iday) 
         throw();

         /** Fundamental routine to convert seconds of day to H:M:S
          * @param sod seconds of day (input)
          * @param hh reference to integer hour (0 <= hh < 24) (output)
          * @param mm reference to integer minutes (0 <= mm < 60) (output)
          * @param sec reference to double seconds (0 <= sec < 60.0) (output)
          */
      static void convertSODtoTime(double sod,
                                   int& hh, 
                                   int& mm, 
                                   double& sec) 
         throw();

         /** Fundamental routine to convert H:M:S to seconds of day
          * @param hh integer hour (0 <= hh < 24) (input)
          * @param mm integer minutes (0 <= mm < 60) (input)
          * @param sec double seconds (0 <= sec < 60.0) (input)
          * @return sod seconds of day (input)
          */
      static double convertTimeToSOD(int hh,
                                     int mm, 
                                     double sec) 
         throw();

         // ----------- Part 12: private functions and member data ----------
         //
   private:
         /// Initialization method.  Used by the constructors.
      void init() 
         throw();

         /// Constructor with direct member data input.
      DayTime(long jd,
              long sod,
              double ms,
              double tol, 
              TimeFrame f = Unknown)
         throw() 
            : jday(jd), mSod(sod), mSec(ms), tolerance(tol), timeFrame(f) 
      {}

         /** Helper routine for add...() functions
          * @param ldd the number of days to add to jday
          * @param lds the number of milliseconds to add to mSod
          * @param ds the number of partial milliseconds to add to mSec
          * @throws DayTimeException if adding these values causes an overflow
          *  of the jday value
          */
      void addLongDeltaTime(long ldd,
                            long lds, 
                            double ds)
         throw(DayTimeException);

         /// integer 'Julian day', = JD+0.5 (0 <= jday <= 3442448)
      long jday; 
          
         /// integer milliseconds of the day (0 <= mSod <= 86400000)
      long mSod;           

         /// double fractional milliseconds (mSec < 1.0)
      double mSec;         

         /// double tolerance used in comparisons (seconds)
      double tolerance;    

         /// @sa TimeFrame
      TimeFrame timeFrame; 

   };   // end class DayTime

      // ----------- Part 13: operator<< -----------
      //
      /**
       * Stream output for DayTime objects.  Typically used for debugging.
       * @param s stream to append formatted DayTime to.
       * @param t DayTime to append to stream \c s.
       * @return reference to \c s.
       */
   std::ostream& operator<<( std::ostream& s,
                             const gpstk::DayTime& t );

      //@}

}  // namespace gpstk

#endif   // GPSTK_DAYTIME_HPP
