#pragma ident "$Id: //depot/sgl/gpstk/dev/src/TimeConstants.hpp#3 $"

#ifndef GPSTK_TIMECONSTANTS_HPP
#define GPSTK_TIMECONSTANTS_HPP

namespace gpstk
{
      /// Add this offset to convert Modified Julian Date to Julian Date.
   const double MJD_TO_JD = 2400000.5;
      /// 'Julian day' offset from MJD
   const long MJD_JDAY = 2400001L;
      /// 'Julian day' of GPS epoch (Jan. 1, 1980).
   const long GPS_EPOCH_JDAY = 2444245L;
      /// Modified Julian Date of GPS epoch (Jan. 1, 1980).
   const long GPS_EPOCH_MJD = 44244L;
      /// Modified Julian Date of UNIX epoch (Jan. 1, 1970).
   const long UNIX_MJD = 40587L;

      /// Seconds per half a GPS week.
   const long HALFWEEK = 302400L;
      /// Seconds per whole GPS week.
   const long FULLWEEK = 604800L;

      /// Weeks per GPS Epoch
   const long WEEKS_PER_EPOCH = 1024L;

      /// Seconds per day.
   const long SEC_PER_DAY = 86400L;
      /// Days per second.
   const double DAY_PER_SEC = 1.0/SEC_PER_DAY;

      /// Milliseconds in a second.
   const long MS_PER_SEC = 1000L;
      /// Seconds per millisecond.
   const double SEC_PER_MS = 1.0/MS_PER_SEC;
   
      /// Milliseconds in a day.
   const long MS_PER_DAY = MS_PER_DAY * SEC_PER_DAY;
      /// Days per milliseconds.
   const double DAY_PER_MS = 1.0/DAY_PER_MS;

      /// Zcounts in a  day.
   const long ZCOUNT_PER_DAY = 57600L;
      /// Days in a Zcount
   const double DAY_PER_ZCOUNT = 1.0/ZCOUNT_PER_DAY;

} // namespace

#endif // GPSTK_TIMECONSTANTS_HPP
