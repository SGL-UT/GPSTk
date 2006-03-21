#pragma ident "$Id: //depot/sgl/gpstk/dev/src/TimeString.hpp#4 $"

#ifndef GPSTK_TIMESTRING_HPP
#define GPSTK_TIMESTRING_HPP

#include "TimeTag.hpp"
#include "CommonTime.hpp"

namespace gpstk
{
   std::string printTime( const TimeTag& t,
                          const std::string& fmt )
      throw( gpstk::StringUtils::StringException );

   std::string printTime( const CommonTime& t,
                          const std::string& fmt )
      throw( gpstk::StringUtils::StringException );
   
      /// Fill the TimeTag object \a btime with time information found in
      /// string \a str formatted according to string \a fmt.
   void scanTime( TimeTag& btime,
                  const std::string& str,
                  const std::string& fmt )
      throw( gpstk::InvalidRequest,
             gpstk::StringUtils::StringException );
   
   void scanTime( CommonTime& t,
                  const std::string& str,
                  const std::string& fmt )
      throw( gpstk::InvalidRequest,
             gpstk::StringUtils::StringException );
   
} // namespace

#endif // GPSTK_TIMESTRING_HPP
