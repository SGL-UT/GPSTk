#pragma ident "$Id: //depot/sgl/gpstk/dev/src/GPSZcount.cpp#9 $"

/**
 * @file GPSZcount.cpp
 * gpstk::GPSZcount - encapsulates time according to full GPS Week and Z-count.
 */

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

#include <limits>

#include "GPSZcount.hpp"
#include "StringUtils.hpp"

using namespace std;
using gpstk::StringUtils::asString;

namespace gpstk
{
   const long GPSZcount::ZCOUNT_MINUTE = 40;
   const long GPSZcount::ZCOUNT_HOUR = 2400;
   const long GPSZcount::ZCOUNT_DAY = 57600;
   const long GPSZcount::ZCOUNT_WEEK = 403200;
   
   GPSZcount::GPSZcount(short inWeek, 
                        long inZcount) 
      throw(gpstk::InvalidParameter) 
   {
      try
      {
         setWeek(inWeek);
         setZcount(inZcount);
      }
      catch(gpstk::InvalidParameter& ip)
      {
         GPSTK_RETHROW(ip);
      }
   }

   GPSZcount::GPSZcount(long inFullZcount)
      throw(gpstk::InvalidParameter)
   {
      try
      {
         setFullZcount(inFullZcount);
      }
      catch(gpstk::InvalidParameter& ip)
      {
         GPSTK_RETHROW(ip);
      }
   }

   GPSZcount::GPSZcount(const GPSZcount& right)
      throw() 
   {
      operator=(right) ;
   }
   
   GPSZcount& GPSZcount::setWeek(short inWeek)
      throw(gpstk::InvalidParameter)
   {
      if( inWeek < 0 )
      {
         gpstk::InvalidParameter ip("GPS Week invalid: " +
                                    asString<short>(inWeek)) ;
         GPSTK_THROW(ip) ;
      }
      week = inWeek;
      return *this;
   }
   
   GPSZcount& GPSZcount::setZcount(long inZcount)
      throw(gpstk::InvalidParameter)
   {
      if(validZcount(inZcount) != 0)
      {
         gpstk::InvalidParameter ip("GPS Z-count invalid: " +
                                    asString<long>(inZcount)) ;
         GPSTK_THROW(ip) ;
      }
      zcount = inZcount ;
      return *this ;
   }
   
   GPSZcount& GPSZcount::setFullZcount(long inZcount)
         throw(gpstk::InvalidParameter) 
   {
      try
      {
         setZcount(inZcount & 0x7FFFFL) ;        // 19-bit mask

         // A 10-bit value will always be within constraints for a GPS week, so
         // there's no need to test it.
         setWeek((inZcount >> 19) & 0x3FFL) ;   // 10-bit mask

      }
      catch(gpstk::InvalidParameter& ip)
      {
         ip.addText("GPS Full Z-count invalid: " + asString<long>(inZcount)) ;
         GPSTK_RETHROW(ip) ;
      }
      return *this ;
   }

   GPSZcount& GPSZcount::addWeeks(short inWeeks)
      throw(gpstk::InvalidRequest) 
   {
      if (inWeeks == 0)
      {
         return *this ;
      }

      try
      {
         return setWeek(week + inWeeks) ;
      }
      catch(gpstk::InvalidParameter& ip)
      {
         gpstk::InvalidRequest ir(ip) ;
         ir.addText("Addition of " + asString(inWeeks) + 
                    " weeks renders this object invalid.") ;
         GPSTK_THROW(ir) ;
      }
   }

   GPSZcount& GPSZcount::addZcounts(long inZcounts)
      throw(gpstk::InvalidRequest) 
   {
      if (inZcounts == 0)
      {
         return *this ;
      }

      short originalWeek(week) ;
      long originalZcount(zcount) ;

      try
      {
            // First, do week modifications.
         addWeeks(inZcounts / ZCOUNT_WEEK) ; 

            // Now, take care of Z-counts.
         long tmp = zcount + (inZcounts % ZCOUNT_WEEK) ;

         if (tmp < 0)
         {
            addWeeks(-1);
            tmp += ZCOUNT_WEEK ;
         }
         else if (tmp >= ZCOUNT_WEEK)
         {
            addWeeks(1);
            tmp -= ZCOUNT_WEEK ;
         }
         
         setZcount(tmp) ;
         return *this ;

      }
      catch(gpstk::InvalidRequest& ir)
      {
         setWeek(originalWeek) ;
         setZcount(originalZcount) ; 
         ir.addText("Did not add " + asString(inZcounts) + " Z-counts.") ;
         GPSTK_RETHROW(ir) ;
         
      }
      catch(gpstk::InvalidParameter& ip)
      {
         setWeek(originalWeek) ;
         setZcount(originalZcount) ; 
         gpstk::InvalidRequest ir(ip) ;
         ir.addText("Did not add " + asString(inZcounts) + " Z-counts.") ;
         GPSTK_THROW(ir) ;         
      }
   }
   
   GPSZcount GPSZcount::operator++(int) 
      throw(gpstk::InvalidRequest)
   {
      GPSZcount temp = *this ;
      ++(*this) ;
      return temp ;
   }

   GPSZcount& GPSZcount::operator++() 
      throw(gpstk::InvalidRequest)
   {
      return addZcounts(1);
   }

   GPSZcount GPSZcount::operator--(int)
      throw(gpstk::InvalidRequest)
   {
      GPSZcount temp = *this ;
      --(*this) ;
      return temp ;
   }

   GPSZcount& GPSZcount::operator--()
      throw(gpstk::InvalidRequest)
   {
      return addZcounts(-1);
   }
   
   GPSZcount GPSZcount::operator+(long inZcounts) const
      throw(gpstk::InvalidRequest)
   {
      return GPSZcount(*this).addZcounts(inZcounts);
   }
   
   GPSZcount GPSZcount::operator-(long inZcounts) const
      throw(gpstk::InvalidRequest)
   {
      return operator+(-inZcounts);
   }

   double GPSZcount::operator-(const GPSZcount& right) const
      throw() 
   {
      return (double(week) - right.week) * ZCOUNT_WEEK 
         + (zcount - right.zcount) ;
   }

   GPSZcount& GPSZcount::operator+=(long inZcounts)
      throw(gpstk::InvalidRequest)
   {
      return addZcounts(inZcounts);
   }

   GPSZcount& GPSZcount::operator-=(long inZcounts)
      throw(gpstk::InvalidRequest)
   {
      return addZcounts(-inZcounts);
   }

   GPSZcount& GPSZcount::operator=(const GPSZcount& right)
      throw()
   {
      week = right.week;
      zcount = right.zcount;
      return *this;
   }

   bool GPSZcount::operator<(const GPSZcount& right) const
      throw()
   {
      if (week < right.week)
      {
         return true;
      }
      if (week == right.week && 
          zcount < right.zcount)
      {
         return true;
      }
      return false;
   }
   
   bool GPSZcount::operator>(const GPSZcount& right) const
      throw()
   {
      if (week > right.week)
      {
         return true;
      }
      if (week == right.week &&
          zcount > right.zcount)
      {
         return true;
      }
      return false;
   }
   
   bool GPSZcount::operator==(const GPSZcount& right) const
      throw()
   {
      if (week == right.week &&
          zcount == right.zcount)
      {
         return true;
      }
      return false;
   }
   
   bool GPSZcount::operator!=(const GPSZcount& right) const
      throw()
   {
      return (! operator==(right));
   }

   bool GPSZcount::operator<=(const GPSZcount& right) const
      throw()
   {
      return (! operator>(right));
   }
   
   bool GPSZcount::operator>=(const GPSZcount& right) const
      throw()
   {
      return (! operator<(right));
   }

   GPSZcount::operator std::string() const
      throw() 
   {
      return asString<short>(week) + "w" + asString<long>(zcount) + "z";
   }

   bool GPSZcount::inSameTimeBlock(const GPSZcount& other,
                                   unsigned long inZcountBlock,
                                   unsigned long inZcountOffset)
      throw()
   {
      if (inZcountBlock < ZCOUNT_WEEK)
      {
            // Make sure that we're in the same week, and then check to see if 
            // we're in the same time block
         if ( (getWeek() == other.getWeek()) &&
              (((getZcount() - inZcountOffset) / inZcountBlock) ==
               ((other.getZcount() - inZcountOffset) / inZcountBlock)) ) 
         {
            return true ;
         }
      }
      else // inZcountBlock >= ZCOUNT_WEEK
      {
            // Compare using the total number of Z-counts.
         if (long((getTotalZcounts() - inZcountOffset) / inZcountBlock) ==
             long((other.getTotalZcounts() - inZcountOffset) / inZcountBlock))
         {
            return true ;
         }
      }
      
      return false ;
   }
   
   void GPSZcount::dump(std::ostream& out,
                        short level) const
      throw()
   {
      switch(level)
      {
         case 0:
            out << week << "w" << zcount << "z" << flush;
            break;
         case 1:
         default:
            out << "GPS Full Week: " << setw(6) << week << endl
                << "GPS Z-count:   " << setw(6) << zcount << endl;
            break;
      }               
   }

   long GPSZcount::validZcount(long z) 
      throw()
   {
      if (z < 0)
      {
         return z;
      }
      if (z >= GPSZcount::ZCOUNT_WEEK)
      {
         return (z - (GPSZcount::ZCOUNT_WEEK - 1));
      }
      return 0; // z is OK
   }
   
   std::ostream& operator<<(std::ostream& s, 
                            const gpstk::GPSZcount& z)
   {
      z.dump(s, 0) ;
      return s;
   }

} // namespace gpstk
