#pragma ident "$Id: //depot/sgl/gpstk/dev/src/CommonTime.cpp#1 $"

#include "CommonTime.hpp"
#include "TimeConstants.hpp"

#include "MathBase.hpp"
#include "StringUtils.hpp"

namespace gpstk
{
      // 'julian day' of earliest epoch expressible by CommonTime; 1/1/4713 B.C.
   const long CommonTime::BEGIN_LIMIT_JDAY = 0;
      // 'julian day' of latest 'julian day' expressible by CommonTime, 
      // 1/1/4713 A.D.
   const long CommonTime::END_LIMIT_JDAY = 3442448;

      // earliest representable CommonTime
   const CommonTime 
   CommonTime::BEGINNING_OF_TIME( CommonTime::BEGIN_LIMIT_JDAY, 0, 0.0 );
      // latest representable CommonTime
   const CommonTime 
   CommonTime::END_OF_TIME( CommonTime::END_LIMIT_JDAY, 0, 0.0 ) ;

   CommonTime::CommonTime( const CommonTime& right )
      throw()
         : m_day( right.m_day ), m_msod( right.m_msod ), m_fsod( right.m_fsod )
   {}

   CommonTime& CommonTime::operator=( const CommonTime& right )
      throw()
   {
      m_day = right.m_day;
      m_msod = right.m_msod;
      m_fsod = right.m_fsod;
      return *this; 
   }

   CommonTime& CommonTime::set( long day,
                                long sod,
                                double fsod )
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

      return *this;
   }

   CommonTime& CommonTime::set( long day,
                                double sod )
      throw( gpstk::InvalidParameter )
   {
         // separate whole and fractional seconds, then use set()
      long sec = static_cast<long>( sod );
      sod -= sec;

      return set( day, sec, sod );
   }

   CommonTime& CommonTime::set( double day = 0.0 )
      throw( gpstk::InvalidParameter )
   {
         // separate whole and fractional days
      long lday = static_cast<long>( day );
      double sec = ( day - lday ) * SEC_PER_DAY;
      return set( lday, sec );
   }

   CommonTime& CommonTime::setInternal( long day,
                                        long msod,
                                        double fsod )
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
      
      m_day = day;
      m_msod = msod;
      m_fsod = fsod;
      
      return *this;
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
                         double& sod ) const
      throw()
   {
      day = m_day;
      sod = m_msod / MS_PER_SEC + m_fsod;
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

   double CommonTime::operator-( const CommonTime& right ) const
      throw()
   {
      return ( SEC_PER_DAY * static_cast<double>( m_day - right.m_day ) +
               SEC_PER_MS * static_cast<double>( m_msod - right.m_msod ) + 
               m_fsod - right.m_fsod  ) ;
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
      long days=0, ms=0;
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
         days = seconds / SEC_PER_DAY;
         seconds -= days * SEC_PER_DAY;  // seconds %= SEC_PER_DAY
      }
      add( days, seconds * MS_PER_SEC, 0. );
      
         // How about this?
         // add( seconds / SEC_PER_DAY,
         //      seconds % SEC_PER_DAY * MS_PER_SEC,
         //      0 );

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

      /// protected functions
   bool CommonTime::add( long days, 
                         long msod,
                         double fsod )
      throw()
   {
      m_day += days;
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
         m_day += day;
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
         m_msod += MS_PER_DAY;
         --m_day;
      }
      
      return ( ( m_day >= BEGIN_LIMIT_JDAY ) && 
               ( m_day <  END_LIMIT_JDAY ) );
   }

} // namespace
