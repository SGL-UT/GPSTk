#include <iostream>
#include "DayTime.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{

   try {
      
      DayTime time;

      cout << "Hello world!" << endl;
      cout << "   The current GPS week is " << time.GPSfullweek() << endl;
      cout << "   The day of the GPS week is " << time.GPSday() << endl;
      cout << "   The seconds of the GPS week is " << time.GPSsecond() << endl;

   }
   catch( Exception error)
   {
      cout << error << endl;
      exit(-1);
   }

   exit(0);
}
