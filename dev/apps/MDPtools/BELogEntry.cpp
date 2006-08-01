#pragma ident "$Id$"


//lgpl-license START
//lgpl-license END

//dod-release-statement START
//dod-release-statement END

/**
 * @file BELogEntry.cpp
 */
#include "DayTime.hpp"
#include "BELogEntry.hpp"

namespace gpstk
{
   using namespace std;
   using namespace gpstk;

      // Initialize the static const.
   const std::string BELogEntry::header = "PRN Earliest SF 1 HOW !                 Toe                      IODC #Collected";
   
   BELogEntry::BELogEntry( const gpstk::EngEphemeris ee )
   {
      HOWSF1 = ee.getTransmitTime();
      Toe = ee.getEpochTime();
      PRN_ID = ee.getPRNID();
      IODC = ee.getIODC();
      count = 1;
      
      long shortweek = (long) ee.getFullWeek();
      shortweek &= 0x0000001F;
      long sixteenSecCount = long (Toe.GPSsow() / 16.0);
      
         // The purpose of the key is to enable placing these objects
         // into SV-specific maps ordered by 
         //    Primary :week
         //    Secondary :SOW
         //    Tertiary: IODC
         // The latter (IODC) is actually only a part of the key so as to
         // enforce uniqueness
      key = shortweek << 26 |
            sixteenSecCount << 10 | IODC;
   }
   
   unsigned long BELogEntry::getKey() const { return(key); }
   DayTime BELogEntry::getHOW() const { return(HOWSF1); }
   void BELogEntry::increment()    { count++; }
   
   std::string BELogEntry::getStr() const
   {
      std::string timeFmt1 = "%02m/%02d/%02y %02H:%02M:%02S";
      std::string timeFmt2 = "%02m/%02d/%02y %02H:%02M:%02S %03j %5.0s %04F %6.0g";
      char line[100];
      sprintf( line, " %02d %s ! %s 0x%03X %4d",
         PRN_ID, HOWSF1.printf( timeFmt1 ).c_str(),
                 Toe.printf(timeFmt2).c_str(),
                 IODC,count);
      std::string retStr(line);
      return(retStr);
   }
   
}   // namespace
