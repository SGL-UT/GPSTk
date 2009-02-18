#pragma ident "$Id$"

#include "Exception.hpp"
#include "MJD.hpp"
#include "JulianDate.hpp"
#include "GPSWeekSecond.hpp"
#include "GPSWeekZcount.hpp"
#include "CivilTime.hpp"
#include "YDSTime.hpp"
#include "UnixTime.hpp"
#include "ANSITime.hpp"

#include "SP3Header.hpp"
#include "SP3Data.hpp"
#include "SP3Stream.hpp"

#include <string>
#include <ctime>

using namespace std;
using namespace gpstk;

int main(int argc, char **argv)
{
   try {
      time_t t;
      time(&t);
      struct tm  *ltod;
      ltod = localtime(&t);
      CivilTime ct;
      ct = CivilTime(1900+ltod->tm_year, ltod->tm_mon+1, ltod->tm_mday,
                        ltod->tm_hour, ltod->tm_min, ltod->tm_sec,
                        TimeSystem::TAI);

      cout << "MJD/TAI: " << MJD(ct) << endl;
      cout << "JulianDate/TAI: " << JulianDate(ct) << endl;
      ct.setTimeSystem(TimeSystem::GPS);
      cout << "GPSWeekSecond/GPS: " << GPSWeekSecond(ct) << endl;
      cout << "GPSWeekZcount/GPS: " << GPSWeekZcount(ct) << endl;
      TimeSystem sys = ct.getTimeSystem();
      cout << "GPS time system is " << sys.asString() << endl;
      //cout << "There are " << sys.getCount() << " timesystems." << endl;
      for(int i=-1; i<10; i++) {
         sys = TimeSystem(i);
         string str = sys.asString();
         cout << "Time system " << i << " is " << str;
         sys.fromString(str);
         str = sys.asString();
         cout << " = " << str << endl;
      }
      sys = 3;
      cout << "Can sys be 3 ? " << sys.asString() << endl;
      //sys = TimeSystem::Count;
      //cout << "Can sys be Count ? " << sys.asString() << endl;
      sys = 17;
      cout << "Can sys be 17 ? " << sys.asString() << endl;
      ct.setTimeSystem(TimeSystem::UTC);
      cout << "Civil/UTC: " << CivilTime(ct) << endl;
      ct.setTimeSystem(TimeSystem::Any);
      cout << "Common/Any: " << CommonTime(ct) << endl;
      ct.setTimeSystem(TimeSystem::GLO);
      cout << "YDS/GLO: " << YDSTime(ct) << endl;
      ct.setTimeSystem(TimeSystem::GAL);
      cout << "Unix/GLO: " << UnixTime(ct) << endl;
      ct.setTimeSystem(static_cast<TimeSystem>(17));
      cout << "ANSI/17: " << ANSITime(ct) << endl;

      UnixTime ut(1234567890,0,TimeSystem::TAI);
      cout << "Unix b-day " << CivilTime(ut) << endl;
      ut.setTimeSystem(TimeSystem::GAL);
      cout << "Unix b-day " << CivilTime(ut) << endl;
   }
   catch(Exception& e) {
      cout << "Exception: " << e.what() << endl;
   }

   return 0;
}
