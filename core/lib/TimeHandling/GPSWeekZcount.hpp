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

#ifndef GPSTK_GPSWEEKZCOUNT_HPP
#define GPSTK_GPSWEEKZCOUNT_HPP

#include "GPSWeek.hpp"
#include "TimeConstants.hpp"
#include "TimeSystem.hpp"

namespace gpstk
{
      /// @ingroup TimeHandling
      //@{

      /**
       * This class encapsulates the "Full GPS Week and GPS Z-count" time
       * representation.
       */
   class GPSWeekZcount : public GPSWeek
   {
   public:
         /// This is just a 19-bit mask.
      static const unsigned int bits19 = 0x7FFFF;

         /**
          * @name GPSWeekZcount Basic Operations
          */
         //@{
         /**
          * Default Constructor.
          * All elements are initialized to zero.
          */
      GPSWeekZcount( int w = 0,
                     int z = 0,
                     TimeSystem ts = TimeSystem::GPS )
            : GPSWeek( w ), zcount( z )
      { timeSystem = ts; }

         /**
          * Copy Constructor.
          * @param right a reference to the GPSWeekZcount object to copy
          */
      GPSWeekZcount( const GPSWeekZcount& right )
            : GPSWeek( right.week ), zcount( right.zcount )
      { timeSystem = right.timeSystem; }

         /**
          * Alternate Copy Constructor.
          * Takes a const TimeTag reference and copies its contents via
          * conversion to CommonTime.
          * @param right a const reference to the BasicTime object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      GPSWeekZcount( const TimeTag& right )
      {
         convertFromCommonTime( right.convertToCommonTime() );
      }

         /**
          * Alternate Copy Constructor.
          * Takes a const CommonTime reference and copies its contents via
          * the convertFromCommonTime method.
          * @param right a const reference to the CommonTime object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      GPSWeekZcount( const CommonTime& right )
      {
         convertFromCommonTime( right );
      }

         /**
          * Assignment Operator.
          * @param right a const reference to the GPSWeekZcount to copy
          * @return a reference to this GPSWeekZcount
          */
      GPSWeekZcount& operator=( const GPSWeekZcount& right );

         /// Virtual Destructor.
      virtual ~GPSWeekZcount()
      {}
         //@}

         // The following functions are required by TimeTag.
      virtual CommonTime convertToCommonTime() const;

      virtual void convertFromCommonTime( const CommonTime& ct );

         /// This function formats this time to a string.  The exceptions
         /// thrown would only be due to problems parsing the fmt string.
      virtual std::string printf( const std::string& fmt ) const;

         /// This function works similarly to printf.  Instead of filling
         /// the format with data, it fills with error messages.
      virtual std::string printError( const std::string& fmt) const;

         /**
          * Set this object using the information provided in \a info.
          * @param info the IdToValue object to which this object shall be set.
          * @return true if this object was successfully set using the
          *  data in \a info, false if not.
          */
      virtual bool setFromInfo( const IdToValue& info );

         /// Return a string containing the characters that this class
         /// understands when printing times.
      inline virtual std::string getPrintChars() const
      {
         return GPSWeek::getPrintChars() + "wzZcC";
      }

         /// Return a string containing the default format to use in printing.
      inline virtual std::string getDefaultFormat() const
      {
         return GPSWeek::getDefaultFormat() + " %06Z %P";
      }

      virtual bool isValid() const
      {
         return ( GPSWeek::isValid() && zcount < ZCOUNT_PER_WEEK );
      }

      inline virtual void reset()
      {
         GPSWeek::reset();
         zcount = 0;
      }

         /// @name Special Zcount-related Methods.
         /// @note The 29- and 32-bit Zcounts cannot represent time from
         /// GPS weeks over 1023 and 8191 respectively.
         //@{
      inline unsigned int getZcount29() const
      {
         return (getWeek10() << 19) | zcount;
      }

      inline unsigned int getZcount32() const
      {
         return (week << 19) | zcount;
      }

      GPSWeekZcount& setZcount29(unsigned int z);

      GPSWeekZcount& setZcount32(unsigned int z);
         //@}

      inline virtual unsigned int getDayOfWeek() const
      {
         return static_cast<unsigned int>(zcount) / ZCOUNT_PER_DAY;
      }

         /**
          * @name GPSWeekZcount Comparison Operators
          * All comparison operators have a parameter "right" which corresponds
          *  to the GPSWeekZcount object to the right of the symbol.
          * All comparison operators are const and return true on success
          *  and false on failure.
          */
         //@{
         //
      inline bool operator==( const GPSWeekZcount& right ) const
      {
         return ( GPSWeek::operator==(right) && zcount == right.zcount );
      }

      inline bool operator!=( const GPSWeekZcount& right ) const
      {
         return ( !operator==( right ) );
      }

      inline bool operator<( const GPSWeekZcount& right ) const
      {
         if( GPSWeek::operator<(right) )
         {
            return true;
         }
         if( GPSWeek::operator>(right) )
         {
            return false;
         }
         if( zcount < right.zcount )
         {
            return true;
         }
         return false;
      }

      inline bool operator>( const GPSWeekZcount& right ) const
      {
         return ( !operator<=( right ) );
      }

      inline bool operator<=( const GPSWeekZcount& right ) const
      {
         return ( operator<( right ) || operator==( right ) );
      }

      inline bool operator>=( const GPSWeekZcount& right ) const
      {
         return ( !operator<( right ) );
      }

         //@}

         /** Calculate the total number of Z-counts in this object.
          * @return weeks * ZCOUNT_PER_WEEK + zcount
          */
      unsigned long getTotalZcounts() const
         throw()
      { return (unsigned long)week * ZCOUNT_PER_WEEK + zcount; }

         /** Add the given number of weeks to the current value.
          * @param[in] inWeeks the number of weeks to add to the current value.
          * @return a reference to this object
          * @throw gpstk::InvalidRequest if adding inWeeks would
          *   render this object invalid. */
      GPSWeekZcount& addWeeks(short inWeeks)
         throw(gpstk::InvalidRequest);

         /** Add the given number of Z-counts to the current value.
          * This may cause a roll-(over/under) of the Z-count and
          * appropriate modification of the week.
          * @param[in] inZcounts the number of Z-counts to add to the
          *   current value.
          * @return a reference to this object.
          * @throw gpstk::InvalidRequest if adding inZcounts would
          *   render this object invalid. */
      GPSWeekZcount& addZcounts(long inZcounts)
         throw(gpstk::InvalidRequest);

         /** Postfix increment the Z-count in this object (x++).  This
          * may also cause the roll-over of the Z-count and
          * incrementing of the week.
          * @return a GPSWeekZcount with the value of this object
          *   before the increment.
          * @throw gpstk::InvalidRequest if adding inZcounts would
          *   render this object invalid. */
      GPSWeekZcount operator++(int)
         throw(gpstk::InvalidRequest);

         /** Prefix increment the Z-count in this object (++x). This
          * may also cause the roll-over of the Z-count and
          * incrementing of the week.
          * @return a reference to this object.
          * @throw gpstk::InvalidRequest if adding inZcounts would
          *   render this object invalid. */
      GPSWeekZcount& operator++()
         throw(gpstk::InvalidRequest);

         /** Postfix decrement the Z-count in this object (x--).  This
          * may also cause the roll-under of the Z-count and
          * decrementing of the week.
          * @return a GPSWeekZcount with the value of this object
          *   before the decrement.
          * @throw gpstk::InvalidRequest if a Z-count decrement would
          *   render this object invalid. */
      GPSWeekZcount operator--(int)
         throw(gpstk::InvalidRequest);

         /** Prefix decrement the Z-count in this object (--x). This
          * may also cause the roll-under of the Z-count and
          * decrementing of the week.
          * @return a reference to this object.
          * @throw gpstk::InvalidRequest if a Z-count decrement would
          *   render this object invalid. */
      GPSWeekZcount& operator--()
         throw(gpstk::InvalidRequest);

         /** Add the given number of Z-counts to the current value.
          * This may cause a roll-(over/under) of the Z-count and
          * appropriate modification of the week.
          * @param[in] inZcounts the number of Z-counts to add to the
          *   current value.
          * @return a modified GPSWeekZcount object.
          * @throw gpstk::InvalidRequest if adding inZcounts would
          *   render this object invalid. */
      GPSWeekZcount operator+(long inZcounts) const
         throw(gpstk::InvalidRequest);

         /** Subtract the given number of Z-counts to the current value.
          * This may cause a roll-(over/under) of the Z-count and
          * appropriate modification of the week.
          * @param[in] inZcounts the number of Z-counts to subtract from the
          *   current value.
          * @return a modified GPSWeekZcount object.
          * @throw gpstk::InvalidRequest if adding inZcounts would
          *   render this object invalid. */
      GPSWeekZcount operator-(long inZcounts) const
         throw(gpstk::InvalidRequest);

         /** Compute the time difference between this object and \a right.
          * @param[in] right the GPSWeekZcount to subtract from this object.
          * @return the number of Z-counts between this object and \a right */
      long operator-(const GPSWeekZcount& right) const
         throw();

         /** Add the given number of Z-counts to the current value.
          * This may cause a roll-(over/under) of the Z-count and
          * appropriate modification of the week.
          * @param[in] inZcounts the number of Z-counts to add to the
          *   current value.
          * @return a reference to this object.
          * @throw gpstk::InvalidRequest if adding inZcounts would
          *   render this object invalid. */
      GPSWeekZcount& operator+=(long inZcounts)
         throw(gpstk::InvalidRequest);

         /** Subtract the given number of Z-counts to the current value.
          * This may cause a roll-(over/under) of the Z-count and
          * appropriate modification of the week.
          * @param[in] inZcounts the number of Z-counts to subtract from the
          *   current value.
          * @return a reference to this object.
          * @throw gpstk::InvalidRequest if adding inZcounts would
          *   render this object invalid. */
      GPSWeekZcount& operator-=(long inZcounts)
         throw(gpstk::InvalidRequest);

         /**
          * This is a test of whether or not this object and the given
          * GPSWeekZcount object are within the same time-block.  Say
          * you need to find out if the two GPSWeekZcounts are:
          * ... in the same day:    inZcountBlock == ZCOUNT_PER_DAY
          * ... or the same minute: inZcountBlock == ZCOUNT_PER_MINUTE   etc.
          * For inZcountBlock < ZCOUNT_WEEK, block start at the
          *   beginning of the week.
          * For inZcountBlock >= ZCOUNT_WEEK, blocks start at the
          *   beginning of GPS week 0.
          * inZcountOffset allows checking of times off of the usual
          *   boundaries, i.e. in the same day, where a day is defined
          *   as starting at noon instead of midnight, or in the same
          *   minute where a minute starts at 23 seconds instead of
          *   zero.
          * @param[in] other The other GPSWeekZcount object.
          * @param[in] inZcountBlock The number of Z-counts in a time-block.
          * @param[in] inZcountOffset The number of Z-counts to offset
          *   the time-block (default = 0).
          * @return true if this object and \a other are in the same time-block.
          */
      bool inSameTimeBlock(const GPSWeekZcount& other,
                           unsigned long inZcountBlock,
                           unsigned long inZcountOffset = 0)
         throw();

      unsigned int zcount;
   };

      //@}

} // namespace

#endif // GPSTK_GPSWEEKZCOUNT_HPP
