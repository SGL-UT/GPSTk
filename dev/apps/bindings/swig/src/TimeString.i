#pragma ident "$Id$"
#ifndef GPSTK_TIMESTRING_HPP
#define GPSTK_TIMESTRING_HPP
#include "TimeTag.hpp"
#include "CommonTime.hpp"

namespace gpstk
{
   std::string printTime( const CommonTime& t,
                          const std::string& fmt )
      throw( gpstk::StringUtils::StringException );

   template <class TimeTagType>
   std::string printAs( const CommonTime& t,
                        const std::string& fmt )
      throw( gpstk::StringUtils::StringException )
   {
      TimeTagType ttt;
      try
      {
         ttt.convertFromCommonTime(t);
         return ttt.printf(fmt);
      }
      catch (InvalidRequest& ir)
      {
         return ttt.printError(fmt);
      }
   } 

   // void scanTime( TimeTag& btime,
   //                const std::string& str,
   //                const std::string& fmt )
   //    throw( gpstk::InvalidRequest,
   //           gpstk::StringUtils::StringException );
   
   // void scanTime( CommonTime& t,
   //                const std::string& str,
   //                const std::string& fmt )
   //    throw( gpstk::InvalidRequest,
   //           gpstk::StringUtils::StringException );

   // void mixedScanTime( CommonTime& t,
   //                     const std::string& str,
   //                     const std::string& fmt )
   //    throw( gpstk::InvalidRequest,
   //           gpstk::StringUtils::StringException );
}
#endif // GPSTK_TIMESTRING_HPP


%inline %{
  
  gpstk::CommonTime scanTime( const std::string& str,
    const std::string& fmt )
  throw( gpstk::InvalidRequest,
    gpstk::StringUtils::StringException ) {
    gpstk::CommonTime m;
    gpstk::scanTime(m, str, fmt);
    return m;
  }

  gpstk::CommonTime mixedScanTime( const std::string& str,
    const std::string& fmt )
  throw( gpstk::InvalidRequest,
    gpstk::StringUtils::StringException ) {
    gpstk::CommonTime m;
    gpstk::mixedScanTime(m, str, fmt);
    return m;
  }

%}

