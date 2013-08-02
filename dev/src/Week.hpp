/// @file Week.hpp  Pure virtual class used to derive (with WeekSecond) XXXWeekSecond
/// for systems XXX (GPS QZS BDS GAL) and GPSWeek (for GPSWeekZcount).

#ifndef GPSTK_WEEK_HPP
#define GPSTK_WEEK_HPP

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

#define timeSystemCheck if(timeSystem != right.timeSystem && \
   timeSystem != TimeSystem::Any && right.timeSystem != TimeSystem::Any) \
   GPSTK_THROW(InvalidRequest( \
   "CommonTime objects not in same time system, cannot be compared"))

#include "TimeTag.hpp"
#include "TimeConverters.hpp"

namespace gpstk
{
   /// This class is an abstract base class inherited by other time-related classes
   /// that use a "week number + seconds of week" formulation; this includes
   /// GPS, GAL, BDS and QZS. It handles only the week number, including rollover
   /// and the modulo operation; class WeekSecond inherits this class and handles
   /// the seconds-of-week.
   /// The class is abstract because it does not fulfill the abstract methods of
   /// the TimeTag class, and because the routines defining the number of bits
   /// in the "ModWeek" and the starting epoch are not defined.
   ///
   /// The Week class inherits from TimeTag and handles the epoch and
   /// N-bit week special cases:
   ///  getEpoch() returns int (week / rollover) or the number of rollovers,
   ///  getModWeek() returns week % rollover or the "short week"
   ///  getEpochModWeek(int& e, int& w) e = getEpoch(); w = getModWeek();
   ///  setEpoch(int e) sets week = (week & bitmask) | (e << Nbits);
   ///  setModWeek(int w) sets week = (week & ~bitmask) | (w & bitmask);
   ///  setEpochModWeek(int e, int w) calls setEpoch(e); setWeek(w);
   ///
   class Week : public TimeTag
   {
   public:

      /// Return the number of bits in the bitmask used to get the ModWeek from the
      /// full week.
      /// This is pure virtual and must be implemented in the derived class;
      /// e.g. GPSWeek::Nbits(void) { static const int n=10; return n; }
      virtual int Nbits(void) const = 0;

      /// Return the bitmask used to get the ModWeek from the full week.
      /// This is pure virtual and must be implemented in the derived class;
      /// e.g. GPSWeek::bitmask(void) { static const int bm=0x3FF; return bm; }
      virtual int bitmask(void) const = 0;

      /// Return the maximum Nbit-week-number minus 1, i.e. the week number at
      /// which rollover occurs.
      virtual int rollover(void) const
         { return bitmask()+1; }

      /// Return the Julian Day (JDAY) of epoch for this system.
      /// This is pure virtual and must be implemented in the derived class;
      /// e.g. GPSWeek::JDayEpoch(void)
      ///   { static const long e=GPS_EPOCH_JDAY; return e; }
      virtual long JDayEpoch(void) const = 0;

      /// Return the greatest week value for which a conversion to or
      /// from CommonTime would work.
      int MAXWEEK(void) const
      {
         static const int mw = (CommonTime::END_LIMIT_JDAY - JDayEpoch())/7;
         return mw;
      }

      /// Constructor.
      Week(int w = 0, TimeSystem ts = TimeSystem::Unknown )
            : week(w)
      { timeSystem = ts; }

      /// Virtual Destructor.
      virtual ~Week()
      {}

      /// Assignment Operator.
      Week& operator=(const Week& right);

         /// @name Comparison Operators.
         //@{
      inline bool operator==(const Week& right) const
      {
         // Any (wildcard) type exception allowed, otherwise must be same time systems
         if(timeSystem != right.timeSystem &&
            timeSystem != TimeSystem::Any && right.timeSystem != TimeSystem::Any)
               return false;

         return (week == right.week);
      }

      inline bool operator!=(const Week& right) const
      {
         timeSystemCheck;
         return (week != right.week);
      }

      inline bool operator<(const Week& right) const
      {
         timeSystemCheck;
         return week < right.week;
      }

      inline bool operator<=(const Week& right) const
      {
         timeSystemCheck;
         return week <= right.week;
      }

      inline bool operator>(const Week& right) const
      {
         timeSystemCheck;
         return week > right.week;
      }

      inline bool operator>=(const Week& right) const
      {
         timeSystemCheck;
         return week >= right.week;
      }
         //@}

      /// @name Special Epoch and Nbit Week Methods.
      /// @todo Should the "set" methods return a reference?
         //@{
      inline virtual unsigned int getEpoch() const
      {
         return week >> Nbits();
      }

      inline virtual unsigned int getModWeek() const
      {
         return week & bitmask();
      }

      inline virtual void getEpochModWeek(unsigned int& e,
                                          unsigned int& w) const
      {
         e = getEpoch();
         w = getModWeek();
      }

      inline virtual void setEpoch(unsigned int e)
      {
         week &= bitmask();
         week |= e << Nbits();
      }

      inline virtual void setModWeek(unsigned int w)
      {
         week &= ~bitmask();
         week |= w & bitmask();
      }

      inline virtual void setEpochModWeek(unsigned int e,
                                          unsigned int w)
      {
         setEpoch(e);
         setModWeek(w);
      }

      inline virtual void adjustToYear(unsigned int y)
      {
         short halfroll = rollover()/2;
         long jd1,jd2;
         int iyear,imon,iday,ep1,ep2;
         jd1 = convertCalendarToJD(y,1,1);
         ep1 = (jd1 - JDayEpoch())/7;
         jd2 = convertCalendarToJD(y,12,31);
         ep2 = (jd2 - JDayEpoch())/7;
         unsigned int mw = getModWeek();

         if(ep1 == ep2)                      // no rollover in given year
            setEpoch(ep1);
         else if(mw <= halfroll)
            setEpoch(ep2);                   // rollover happened before mw -> use ep2
         else
            setEpoch(ep1);                   // rollover happened after mw
      }

         //@}

      /// This function formats this time to a string.  The exceptions
      /// thrown would only be due to problems parsing the fmt string.
      virtual std::string printf( const std::string& fmt ) const;

      /// This function works similarly to printf.  Instead of filling
      /// the format with data, it fills with error messages.
      virtual std::string printError( const std::string& fmt ) const;

      /// Set this object using the information provided in \a info.
      /// @param info the IdToValue object to which this object shall be set.
      /// @return true if this object was successfully set using the
      ///  data in \a info, false if not.
      virtual bool setFromInfo(const IdToValue& info);

      /// Return a string containing the characters that this class
      /// understands when printing times.
      inline virtual std::string getPrintChars() const
      {
         return "EFGP";
      }

      /// Return a string containing the default format to use in printing.
      inline virtual std::string getDefaultFormat() const
      {
         return "%04F";
      }

      virtual bool isValid() const
      {
         return ((week >= 0) && (week <= MAXWEEK()));
      }

      inline virtual void reset()
      {
         week = 0;
      }

      /// Force this interface on this classes descendants.
      virtual unsigned int getDayOfWeek() const = 0;

      // member data
      int week;
   };

} // namespace

#endif // GPSTK_WEEK_HPP
