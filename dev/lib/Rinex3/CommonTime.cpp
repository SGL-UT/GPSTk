#pragma ident "$Id: CommonTime.cpp 1162 2008-03-27 21:18:13Z snelsen $"

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

#include <cmath>
#include "CommonTime.hpp"
#include "MathBase.hpp"
#include "StringUtils.hpp"

//using namespace gpstk;

namespace gpstk
{
      // mSod is sod*FACTOR and mSec is seconds*FACTOR
      // NB FACTOR must be <, and a factor of, 1,000,000
   const long CommonTime::FACTOR = 1000L;

      // Seconds per half a GPS week.
   const long CommonTime::HALFWEEK = 302400L;
      // Seconds per whole GPS week.
   const long CommonTime::FULLWEEK = 604800L;

      // 'julian day' of earliest epoch expressible by CommonTime; 1/1/4713 B.C.
   const long CommonTime::BEGIN_LIMIT_JDAY = 0L;
      // 'julian day' of latest 'julian day' expressible by CommonTime,
      // 1/1/4713 A.D.
   const long CommonTime::END_LIMIT_JDAY = 3442448L;
      // 'Julian day' of GPS epoch (Jan. 1, 1980).
   const long CommonTime::GPS_EPOCH_JDAY = 2444245L;

      // earliest representable CommonTime
   const CommonTime
   CommonTime::BEGINNING_OF_TIME( CommonTime::BEGIN_LIMIT_JDAY, 0, 0.0, Unknown );
      // latest representable CommonTime
   const CommonTime
   CommonTime::END_OF_TIME( CommonTime::END_LIMIT_JDAY, 0, 0.0, Unknown ) ;

      // Seconds per day.
   const long CommonTime::SEC_DAY = 86400L;

   //@{
   /// Default tolerance for time equality, applied to milliseconds.
   const double CommonTime::eps = 4.*std::numeric_limits<double>::epsilon();
   //@}


   CommonTime::CommonTime( const CommonTime& right )
      throw()
     : m_day( right.m_day ), m_msod( right.m_msod ), m_fsod( right.m_fsod ),
       m_timeSystem( right.m_timeSystem )
   {}

   CommonTime& CommonTime::operator=( const CommonTime& right )
      throw()
   {
      m_day  = right.m_day;
      m_msod = right.m_msod;
      m_fsod = right.m_fsod;
      m_timeSystem = right.m_timeSystem;
      return *this;
   }

   CommonTime& CommonTime::set( long day,
                                long sod,
                                double fsod,
                                TimeSystem timeSystem )
      throw( gpstk::InvalidParameter )
   {
         // Use temp variables so that we don't modify our
         // data members until we know these values are good.
      if( day < BEGIN_LIMIT_JDAY || day > END_LIMIT_JDAY )
      {
         gpstk::InvalidParameter ip( "Invalid day: " 
                                     + gpstk::StringUtils::asString( day ) );
         GPSTK_THROW( ip );
      }

      if( sod < 0 || sod >= SEC_PER_DAY )
      {
         gpstk::InvalidParameter ip( "Invalid seconds of day: "
                                     + gpstk::StringUtils::asString( sod ) );
         GPSTK_THROW( ip );
      }

      if( fsod < 0.0 || fsod >= 1 )
      {
         gpstk::InvalidParameter ip( "Invalid fractional-seconds: "
                                     + gpstk::StringUtils::asString( fsod ) );
         GPSTK_THROW( ip );
      }

         // get the number of milliseconds in the fractional seconds argument
      long msec = static_cast<long>( fsod * MS_PER_SEC );
      
         // subtract whole milliseconds to obtain the "fractional milliseconds"
      fsod -= static_cast<double>( msec ) * SEC_PER_MS;

      m_day  = day;
      m_msod = sod * MS_PER_SEC + msec;
      m_fsod = fsod;

      m_timeSystem = timeSystem;

      return *this;
   }

   CommonTime& CommonTime::set( long day,
                                double sod,
                                TimeSystem timeSystem )
      throw( gpstk::InvalidParameter )
   {
         // separate whole and fractional seconds, then use set()
      long sec = static_cast<long>( sod );
      sod -= sec;

      return set( day, sec, sod, timeSystem );
   }

  CommonTime& CommonTime::set( double day = 0.0,
                               TimeSystem timeSystem )
      throw( gpstk::InvalidParameter )
   {
         // separate whole and fractional days
      long lday = static_cast<long>( day );
      double sec = ( day - lday ) * SEC_PER_DAY;
      return set( lday, sec, timeSystem );
   }

   CommonTime& CommonTime::setInternal( long day,
                                        long msod,
                                        double fsod,
                                        TimeSystem timeSystem )
      throw( gpstk::InvalidParameter )
   {
      if( day < BEGIN_LIMIT_JDAY || day > END_LIMIT_JDAY )
      {
         gpstk::InvalidParameter ip( "Invalid day: " 
                                     + gpstk::StringUtils::asString( day ) );
         GPSTK_THROW( ip );
      }

      if( msod < 0 || msod >= MS_PER_DAY )
      {
         gpstk::InvalidParameter ip( "Invalid milliseconds of day: "
                                     + gpstk::StringUtils::asString( msod ) );
         GPSTK_THROW( ip );
      }

      if( fsod < 0.0 || fsod >= SEC_PER_MS )
      {
         gpstk::InvalidParameter ip( "Invalid fractional-milliseconds: "
                                     + gpstk::StringUtils::asString( fsod ) );
         GPSTK_THROW( ip );
      }

      m_day  = day;
      m_msod = msod;
      m_fsod = fsod;

      m_timeSystem = timeSystem;

      return *this;
   }

      // Set the object's time using GPS time.
      // @param fullweek Full (i.e. >10bits) GPS week number.
      // @param sow Seconds of week.
      // @param ts Time system (see #TimeSystem).
      // @return a reference to this object.
   CommonTime& CommonTime::setGPSfullweek(short fullweek,
                                          double sow, 
                                          TimeSystem ts)
      throw( gpstk::InvalidParameter )
   {
      if(fullweek < 0 || 
         sow < 0.0 || 
         sow >= double(FULLWEEK))
      {
         using gpstk::StringUtils::asString ;
         Exception e("Invalid week/seconds-of-week: " 
                     + asString<short>(fullweek)+ "/" 
                     + asString(sow));
         GPSTK_THROW(e);
      }
      m_day = GPS_EPOCH_JDAY + 7 * long(fullweek) + long(sow / SEC_DAY);
      double sod = sow - SEC_DAY * long(sow / SEC_DAY);
      m_msod = long(FACTOR * sod);
      m_fsod = FACTOR * sod - double(m_msod);
//      realignInternals();
      m_timeSystem = ts;
      return *this;
   }

   void CommonTime::get( long& day,
                         long& sod,
                         double& fsod,
                         TimeSystem& timeSystem ) const
      throw()
   {
      day = m_day;
      sod = m_msod / MS_PER_SEC;
      long msec = m_msod - sod * MS_PER_SEC;  // m_msod % MS_PER_SEC
      fsod = static_cast<double>( msec ) * SEC_PER_MS + m_fsod;
      timeSystem = m_timeSystem;
   }

   void CommonTime::get( long& day,
                         long& sod,
                         double& fsod ) const
      throw()
   {
      day = m_day;
      sod = m_msod / MS_PER_SEC;
      long msec = m_msod - sod * MS_PER_SEC;  // m_msod % MS_PER_SEC
      fsod = static_cast<double>( msec ) * SEC_PER_MS + m_fsod;
   }

   void CommonTime::get( long& day,
                         double& sod,
                         TimeSystem& timeSystem ) const
      throw()
   {
      day = m_day;
      sod = m_msod / MS_PER_SEC + m_fsod;
      timeSystem = m_timeSystem;
   }

   void CommonTime::get( long& day,
                         double& sod ) const
      throw()
   {
     day = m_day;
      sod = m_msod / MS_PER_SEC + m_fsod;
   }

  void CommonTime::get( double& day,
                        TimeSystem& timeSystem ) const
      throw()
   {
         // convert everything to days
      day = static_cast<double>( m_day ) + 
            static_cast<double>( m_msod ) * MS_PER_DAY +
            m_fsod * SEC_PER_DAY;
      timeSystem = m_timeSystem;
   }

   void CommonTime::get( double& day ) const
      throw()
   {
         // convert everything to days
      day = static_cast<double>( m_day ) + 
            static_cast<double>( m_msod ) * MS_PER_DAY +
            m_fsod * SEC_PER_DAY;
   }

   double CommonTime::getDays() const
      throw()
   {
      double day;
      get( day );
      return day;
   }

   double CommonTime::getSecondOfDay() const
      throw()
   {
      long day;
      double sod;
      get( day, sod );
      return sod;
   }

      // Get seconds of week.
   double CommonTime::GPSsow() const
      throw()
   {
      return double(GPSday() * SEC_DAY) + secOfDay() ;
   }

      // Get full (>10 bits) week 
   short CommonTime::GPSfullweek() const
      throw()
   {
      return short(double(m_day - GPS_EPOCH_JDAY) / 7) ;
   }

   TimeSystem CommonTime::getTimeSystem() const
      throw()
   {
      return m_timeSystem;
   }

   double CommonTime::operator-( const CommonTime& right ) const
      throw()
   {
      return( SEC_PER_DAY * static_cast<double>( m_day  - right.m_day  ) +
              SEC_PER_MS  * static_cast<double>( m_msod - right.m_msod ) + 
              m_fsod - right.m_fsod ) ; // returns difference regardless of timeSystem
   }
   
   CommonTime CommonTime::operator+( double sec ) const
      throw( gpstk::InvalidRequest )
   {
      return CommonTime( *this ).addSeconds( sec );
   }

   CommonTime CommonTime::operator-( double sec ) const
      throw( gpstk::InvalidRequest )
   {
      return CommonTime( *this ).addSeconds( -sec );
   }

   CommonTime& CommonTime::operator+=( double sec )
      throw( gpstk::InvalidRequest )
   {
      addSeconds( sec );
      return *this;
   }

   CommonTime& CommonTime::operator-=( double sec )
      throw( gpstk::InvalidRequest )
   {
      addSeconds( -sec );
      return *this;
   }

   CommonTime& CommonTime::addSeconds( double seconds )
      throw( InvalidRequest )
   {
      long days = 0, ms = 0;
      if ( ABS(seconds) >= SEC_PER_DAY )
      {
         days = static_cast<long>( seconds * DAY_PER_SEC );
         seconds -= days * SEC_PER_DAY;
      }

      if ( ABS(seconds) >= SEC_PER_MS )
      {
         ms = static_cast<long>( seconds * MS_PER_SEC );
         seconds -= static_cast<double>( ms ) * SEC_PER_MS;
      }

      add(days, ms, seconds);
      return *this;
   }

   CommonTime& CommonTime::addSeconds( long seconds )
      throw( gpstk::InvalidRequest )
   {
      long days( 0 );
      if( ABS( seconds ) > SEC_PER_DAY )
      {
         days  = seconds / SEC_PER_DAY;
         seconds -= days * SEC_PER_DAY;  // seconds %= SEC_PER_DAY
      }
      add( days, seconds * MS_PER_SEC, 0. );

      return *this;
   }

   CommonTime& CommonTime::addDays( long days )
      throw( gpstk::InvalidRequest )
   {
      add( days, 0, 0.0 );
      return *this;
   }

   CommonTime& CommonTime::addMilliseconds( long msec )
      throw( InvalidRequest )
   {
      add( 0, msec, 0.0 );
      return *this;
   }

   bool CommonTime::operator==( const CommonTime& right ) const
      throw()
   {
      return (m_timeSystem == right.m_timeSystem &&
              m_day        == right.m_day        &&
              m_msod       == right.m_msod       &&
              fabs(m_fsod-right.m_fsod) < eps      );
   }

   bool CommonTime::operator!=( const CommonTime& right ) const
      throw()
   {
      return !operator==(right);
   }

   bool CommonTime::operator<( const CommonTime& right ) const
      throw()
   {
      if (m_timeSystem != right.m_timeSystem ||
          m_timeSystem == Unknown || right.m_timeSystem == Unknown)
         throw InvalidRequest("CommonTime objects not in same time system");

      if (m_day < right.m_day)
         return true;
      if (m_day > right.m_day)
         return false;

      if (m_msod < right.m_msod)
         return true;
      if (m_msod > right.m_msod)
         return false;

      if (m_fsod < right.m_fsod)
         return true;

      return false;
   }

   bool CommonTime::operator>( const CommonTime& right ) const
      throw()
   {
      return !operator <=(right);
   }

   bool CommonTime::operator<=( const CommonTime& right ) const
      throw()
   {
      return (operator<(right) || operator==(right));
   }

   bool CommonTime::operator>=( const CommonTime& right ) const
      throw()
   {
      return !operator<(right);
   }

   std::string CommonTime::asString() const
      throw()
   {
      using namespace std;
      ostringstream oss;
      oss << setfill('0') 
          << setw(7) << m_day  << " "
          << setw(8) << m_msod << " "
          << fixed << setprecision(15) << setw(17) << m_fsod
          << " in frame " << m_timeSystem ;
      return oss.str();
   }

      /// protected functions
   bool CommonTime::add( long days,
                         long msod,
                         double fsod )
      throw()
   {
      m_day  += days;
      m_msod += msod;
      m_fsod += fsod;
      return normalize();
   }
                                
   bool CommonTime::normalize()
      throw()
   {
      if( ABS( m_fsod ) >= SEC_PER_MS )
      {
         long ms = static_cast<long>( m_fsod * MS_PER_SEC );
         m_msod += ms;
         m_fsod -= static_cast<double>( ms ) * SEC_PER_MS;
      }

      if( ABS( m_msod ) >= MS_PER_DAY )
      {
         long day = m_msod / MS_PER_DAY;
         m_day  += day;
         m_msod -= day * MS_PER_DAY;
      }

      if( ABS(m_fsod) < 1e-15 )
      {
         m_fsod = 0.0;
      }

      if( m_fsod < 0 )
      {
         m_fsod += 1;
         --m_msod;
      }

      if( m_msod < 0 )
      {
         m_msod = m_msod + MS_PER_DAY;
         --m_day;
      }

      return ( ( m_day >= BEGIN_LIMIT_JDAY ) && 
               ( m_day <  END_LIMIT_JDAY   ) );
   }

   std::ostream& operator<<(std::ostream& o, const CommonTime& ct)
   {
      o << ct.asString();
      return o;
   }

} // namespace
