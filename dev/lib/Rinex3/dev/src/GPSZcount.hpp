#pragma ident "$Id: GPSZcount.hpp 500 2007-04-27 12:02:53Z ocibu $"



/**
 * @file GPSZcount.hpp
 * gpstk::GPSZcount - encapsulates time according to full GPS Week and Z-count.
 */

#ifndef GPSTK_GPSZCOUNT_HPP
#define GPSTK_GPSZCOUNT_HPP

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

// system includes
#include <ostream>

// GPSTK includes
#include "Exception.hpp"

namespace gpstk
{
      /** @addtogroup timegroup */
      //@{

      /**
       * A representation class for GPS Time consisting of
       * a short GPS week, and a long GPS Z-count. 
       * A valid GPS week is in the following range:
       *   0 <= week <= numerical_limits<short>::max()
       * A valid GPS Z-count is in the following range:
       *   0 <= zcount < 403200
       *
       * In Z-count addition, if a specified Z-count value is above or below 
       * the given range of valid Z-counts, the appropriate number of GPS 
       * weeks-worth of Z-counts are added or subtracted from the given value 
       * so that it falls within the acceptable range.  These extra weeks are 
       * then accounted for in the weeks data member.  If the addition or 
       * subtraction of these additional weeks cause the GPS week value to go 
       * out-of-bounds, an InvalidRequest exception is thrown.
       */
   class GPSZcount
   {
   public:
         /// Z-counts per minute (40)
      static const long ZCOUNT_MINUTE;
         /// Z-counts per hour (2400)
      static const long ZCOUNT_HOUR;
         /// Z-counts per day (57600)
      static const long ZCOUNT_DAY;
         /// Z-counts per whole GPS week. (403200)
      static const long ZCOUNT_WEEK;

         /** Separate Week and Z-count Constructor.
          * @param inWeek the GPS week to use in this object.
          * @param inZcount the GPS Z-count to use in this object.
          * @throws gpstk::InvalidParameter if either inWeek or inZcount
          *  are invalid values for their respective types.
          */
      GPSZcount(short inWeek,
                long inZcount)
         throw(gpstk::InvalidParameter) ;
      
         /** Full Z-count Constructor.
          * @param inFullZcount the Full 29-bit GPS Z-count to use in 
          *  this object. (default=0 => week == 0; Z-count == 0)
          * @throws gpstk::InvalidParameter if either inFullZcount is
          *  an invalid value for a full Z-count.
          */
      GPSZcount(long inFullZcount = 0) 
         throw(gpstk::InvalidParameter) ;
      
         /// Destructor.  Does nothing.
      ~GPSZcount() throw() {}
      
         /** Copy Constructor.
          * @param right the GPSZcount object to copy
          */
      GPSZcount(const GPSZcount& right)
         throw() ;
      
         /// GPS week accessor
      short getWeek() const throw()
      { return week ; }
      
         /// GPS Z-count accessor
      long getZcount() const throw()
      { return zcount ; }
      
         /** Construct, from the data members, the Z-count which could have
          *  been broadcast from an SV.
          * @return a Full Z-count (3 MSB unused, next 10 bits = week,
          *  19 LSB = Z-count)
          */
      long getFullZcount() const
         throw() 
      { return (long(getWeek() & 0x3FF) << 19) + getZcount() ; }

         /** Calculate the total number of Z-counts in this object.
          * @return weeks * ZCOUNT_WEEK + zcount
          */
      double getTotalZcounts() const
         throw()
      { return double(getWeek()) * ZCOUNT_WEEK + getZcount() ; }

         /** Set the GPS week.
          * @param inWeek the new value for GPS Week.
          * @return a reference to this object.
          * @throws gpstk::InvalidParameter if inWeek is an invalid value
          * for a GPS week.
          */
      GPSZcount& setWeek(short inWeek) 
         throw(gpstk::InvalidParameter) ;
      
         /** Set the GPS Z-count.
          * @param inZcount the new value for GPS Z-count.
          * @return a reference to this object.
          * @throws gpstk::InvalidParameter if inZcount is an invalid value
          *  for a GPS Z-count.
          */
      GPSZcount& setZcount(long inZcount)
         throw(gpstk::InvalidParameter) ;

         /** Set the GPS Week and Z-count using the value broadcast from an SV.
          * @param inZcount A full Z-count (3 MSB unused, next 10 bits = week,
          *  19 LSB = Z-count)
          * @throws gpstk::InvalidParameter if the extracted Z-count is 
          *  an invalid value for a GPS Z-count.
          */
      GPSZcount& setFullZcount(long inZcount)
         throw(gpstk::InvalidParameter) ;
      
         /** Add the given number of weeks to the current value.
          * @param inWeeks the number of weeks to add to the current value.
          * @return a reference to this object
          * @throws gpstk::InvalidRequest if adding inWeeks would render
          *  this object invalid.
          */
      GPSZcount& addWeeks(short inWeeks)
         throw(gpstk::InvalidRequest) ;
      
         /** Add the given number of Z-counts to the current value.
          *  This may cause a roll-(over/under) of the Z-count and appropriate 
          *  modification of the week.
          * @param inZcounts the number of Z-counts to add to the current 
          *  value.
          * @return a reference to this object
          * @throws gpstk::InvalidRequest if adding inZcounts would render
          *  this object invalid.
          */
      GPSZcount& addZcounts(long inZcounts)
         throw(gpstk::InvalidRequest) ;

         /** Postfix Increment the Z-count in this object (x++).  This may also
          * cause the roll-over of the Z-count and incrementing of the week.
          * @return a GPSZcount with the value of this object before the 
          *  increment
          * @throws gpstk::InvalidRequest if Z-count increment would render
          *  this object invalid.
          */
      GPSZcount operator++(int)
         throw(gpstk::InvalidRequest) ;

         /** Prefix Increment the Z-count in this object (++x).  This may also 
          * cause the roll-over of the Z-count and incrementing of the week.
          * @return a reference to this object.
          * @throws gpstk::InvalidRequest if Z-count increment would render
          *  this object invalid.
          */
      GPSZcount& operator++() 
         throw(gpstk::InvalidRequest) ;
      
         /** Postfix Decrement the Z-count in this object (x--).  This may also
          * cause the roll-under of the Z-count and decrementing of the week.
          * @return a GPSZcount with the value of this object before the 
          *  decrement
          * @throws gpstk::InvalidRequest if a Z-count decrement would render
          *  this object invalid.
          */
      GPSZcount operator--(int) 
         throw(gpstk::InvalidRequest) ;

         /** Prefix Decrement the Z-count in this object (--x).  This may also 
          * cause the roll-under of the Z-count and decrementing of the week.
          * @return a reference to this object.
          * @throws gpstk::InvalidRequest if a Z-count decrement would render
          *  this object invalid.
          */
      GPSZcount& operator--() 
         throw(gpstk::InvalidRequest) ;
      
         /** Add the given number of Z-counts to the current value.
          *  This may cause a roll-(over/under) of the Z-count and appropriate 
          *  modification of the week.
          * @param inZcounts the number of Z-counts to add to the current 
          *  value.
          * @return a modified GPSZcount object
          * @throws gpstk::InvalidRequest if adding inZcounts would render
          *  this object invalid.
          */
      GPSZcount operator+(long inZcounts) const
         throw(gpstk::InvalidRequest) ;
      
         /** Subtract the given number of Z-counts from the current value.
          *  This may cause a roll-(over/under) of the Z-count and appropriate 
          *  modification of the week.
          * @param inZcounts the number of Z-counts to subtract from the 
          *  current value.
          * @return a modified GPSZcount object
          * @throws gpstk::InvalidRequest if subtracting inZcounts would
          *  render this object invalid.
          */
      GPSZcount operator-(long inZcounts) const
         throw(gpstk::InvalidRequest) ;

         /** Compute the time differenct between this object and \a right.
          * @param right the GPSZcount to subtract from this object
          * @return the number of Z-counts between this object and \a right
          * Note that this returns a double as opposed to a long. This is to
          * avoid any overflow issues.
          */
      double operator-(const GPSZcount& right) const
         throw() ;
      
         /** Compute the remainder of the ztime
          * @param right the number to 
          * @return z counts % \a right
          */
      long operator%(const long right) const
         throw() ;

         /** Add the given number of Z-counts to the current value.
          *  This may cause a roll-(over/under) of the Z-count and appropriate 
          *  modification of the week.
          * @param inZcounts the number of Z-counts to add to the current 
          *  value.
          * @return a reference to this object
          * @throws gpstk::InvalidRequest if adding inZcounts would render
          *  this object invalid.
          */
      GPSZcount& operator+=(long inZcounts)
         throw(gpstk::InvalidRequest) ;
      
         /** Subtract the given number of Z-counts from the current value.
          *  This may cause a roll-(over/under) of the Z-count and appropriate 
          *  modification of the week.
          * @param inZcounts the number of Z-counts to subtract from the 
          *  current value.
          * @return a reference to this object
          * @throws gpstk::InvalidRequest if subtracting inZcounts would
          *  render this object invalid.
          */
      GPSZcount& operator-=(long inZcounts)
         throw(gpstk::InvalidRequest) ;
      
         /// Assignment operator.
      GPSZcount& operator=(const GPSZcount& right) 
         throw();
      
         /// Comparison operator (less-than).
      bool operator<(const GPSZcount& right) const
         throw();
         /// Comparison operator (greater-than).
      bool operator>(const GPSZcount& right) const
         throw();
         /// Equality operator.
      bool operator==(const GPSZcount& right) const
         throw();
         /// Inequality operator.
      bool operator!=(const GPSZcount& right) const
         throw();
         /// Comparison operator (less-than or equal-to).
      bool operator<=(const GPSZcount& right) const
         throw();
         /// Comparison operator (greater-than or equal-to).
      bool operator>=(const GPSZcount& right) const
         throw();

         /// Convert this object to a string.
      operator std::string() const
         throw() ;

         /**
          * This is a test of whether or not this object and the  given
          * GPSZcount object are within the same time-block.  Say you need to
          * find out if the two GPSZcounts are:
          * ... in the same day:     inZcountBlock == ZCOUNT_DAY
          * ... or the same minute:  inZcountBlock == ZCOUNT_MINUTE   etc.
          * For inZcountBlock < ZCOUNT_WEEK, blocks start at the beginning of
          *  the week.
          * For inZcountBlock >= ZCOUNT_WEEK, blocks start at the beginning of 
          *  GPS Week 0.
          * inZcountOffset allows checking of times off of the usual boundaries
          * i.e. in the same day where a day is defined as starting at noon
          *  instead of at midnight, or in the same minute where a minute
          *  starts at 23 seconds instead of zero.
          * @param other the other GPSZcount object
          * @param inZcountBlock the number of Z-counts in a time-block
          * @param inZcountOffset the number of Z-counts to offset the 
          *  time-block (default = 0)
          * @return whether this object and /a other are in the same time-block
          */
      bool inSameTimeBlock(const GPSZcount& other,
                           unsigned long inZcountBlock,
                           unsigned long inZcountOffset = 0) 
         throw();

         /** Dump the contents of this object to the given output stream.
          * @param out the output stream to write to
          * @param level the level of verbosity to use (default=0)
          */
      void dump(std::ostream& out,
                short level = 0) const
         throw() ;
      
   protected:
      short week;    ///< GPS full week. (0 <= week)
      long zcount;   ///< GPS Z-count.  (0 <= zcount <= 403199)

         /** Check the validity of the given Z-count.
          * @param z the Z-count to verify
          * @return 0 if valid, negative if below zero, 
          *  positive if above 403199
          */
      static long validZcount(long z)
         throw();
      
   };
   
      /** Stream output operator for GPSZcount objects.
       * @param s stream to append formatted GPSZcount to.
       * @param z GPSZcount to append to stream \c s.
       * @return a reference to \c s.
       */
   std::ostream& operator<<(std::ostream& s,
                            const gpstk::GPSZcount& z) ;

      //@}

} // namespace gpstk

#endif // GPSTK_GPSZCOUNT_HPP
