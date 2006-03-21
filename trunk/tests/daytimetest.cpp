#pragma ident "$Id: //depot/sgl/gpstk/dev/tests/daytimetest.cpp#3 $"
//
// test the daytime implementation
//

#include <iostream>

#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "GPSZcount.hpp"

using namespace std;
using namespace gpstk::StringUtils;

/// prints the given daytime with the given format
void dtft(ostream& s, const gpstk::DayTime& t, const string& fmt)
{
   string fs = fmt;
   string p = t.printf(fmt.c_str());

   s << leftJustify(fs, 30) << " " 
     << p << endl;
}

/// returns 0 if all tests pass
int main()
{
   using gpstk::DayTime;
   
   try
   {
      cout << "BOT:" << DayTime(gpstk::DayTime::BEGINNING_OF_TIME) << endl;
      cout << "EOT:" << DayTime(gpstk::DayTime::END_OF_TIME) << endl;
     
      DayTime dt;
      dt.setSystemTime();
      cout << "Check that the output matches the current UTC time." << endl
           << "string                         printf()" << endl;

      dtft(cout, dt, "mjd:  %Q (%.0Q)");
      dtft(cout, dt, "mjd:  %5.3Q");
      dtft(cout, dt, "mdy:  %02m/%02d/%04Y");
      dtft(cout, dt, "hms:  %02H:%02M:%02S");
      dtft(cout, dt, "hms:  %02H:%02M:%06.3f");
      dtft(cout, dt, "cal:  %A, %B %d, %Y");
      dtft(cout, dt, "week: %F(%G)");
      dtft(cout, dt, "sow:  %g");
      dtft(cout, dt, "sow:  %06.3g");
      dtft(cout, dt, "doy:  %j:%s");
      dtft(cout, dt, "dow:  %w");
      dtft(cout, dt, "z:    %Z (%z)");
      dtft(cout, dt, "unix: %U.%06u");

      cout << endl
           << "The following functions use DayTime::setToString()" << endl;

      string format = "%02m/%02d/%04Y %02H:%02M:%02S";
      string st = dt.printf(format);

      DayTime q;
      q.setToString(st, format);
      dtft(cout, q, format);

      cout << "Tests complete." << endl;
      return 0;
   }
   catch(gpstk::Exception& e)
   {
      cout << e << endl;
   }
   catch(...)
   {
      cout << "Some other exception thrown..." << endl;
   }

   cout << "Exiting with exceptions." << endl;
   return -1;
}
