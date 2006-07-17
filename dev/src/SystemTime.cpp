#include "SystemTime.hpp"
#include "ANSITime.hpp"

namespace gpstk
{
   SystemTime& SystemTime::operator=( const SystemTime& right )
      throw()
   {
      UnixTime::operator=( right );
      return *this;
   }

   SystemTime& SystemTime::check()
      throw()
   {
#if defined(ANSI_ONLY)
      time_t t;
      time( &t );
      *this = ANSITime( t );
#elif defined(WIN32)
      _timeb t;
      _ftime( &t );
      tv.tv_sec = t.time;
      tv.tv_usec = t.millitm * 1000;
#else
      gettimeofday( &tv, NULL );
#endif

      return *this;
   }

} // namespace
