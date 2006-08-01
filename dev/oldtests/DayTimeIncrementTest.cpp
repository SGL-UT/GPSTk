#pragma ident "$Id$"


/* \example DayTimeToleranceTest.cpp
 * This test evaluates user defined comparison tolerance feature of DayTime.
 * It merely increments then decrements DayTime in different ways, then
 * reports to the user the estimate of machine error.
 */

#include <iostream>
#include <iomanip>

#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "icd_200_constants.hpp"

using namespace std;
using namespace gpstk;

#define TEST_METHOD(method, incCount, incValue, diffTolerance)     \
      dtcopy = dtorig;                                             \
      totalIncrements=0;                                           \
      incCountUse = incCount/2;                                    \
      for (long j=0; j<incCountUse; ++j)                           \
      {                                                            \
         dtcopy.method(incValue);                                  \
         totalIncrements++;                                        \
      }                                                            \
      for (long j=0; j<incCountUse; ++j)                           \
      {                                                            \
          dtcopy.method(-incValue);                                \
          totalIncrements++;                                       \
      }                                                            \
      tdiff = dtcopy-dtorig;                                       \
      cout << setw(18) << #method;                                 \
      cout << setw(18) << totalIncrements;                         \
      cout << setw(22) << setprecision(10) << tdiff;               \
      cout << setw(21) << setprecision(8) << tdiff * C_GPS_M;      \
      cout << setw(23) << setprecision(5) << diffTolerance;        \
      cout << endl;                                                \
      cumulativeResult = cumulativeResult &&                       \
                         ( fabs(tdiff)<diffTolerance );           


/// returns 0 if all tests pass
int main()
{
   try
   {
      cout << endl << "Testing DayTime increment safety." << endl << endl;

         // Set the DayTimes using Year, Month, Day, Hour, Minute, Second.
      gpstk::DayTime dtorig(2000,12,1,0,0,0.0), dtcopy;

         // Used to time the test.      
      DayTime startTime;
      bool cumulativeResult = true;
      double tdiff;
      long totalIncrements=0; 
      long incCountUse;
      
      cout << setw(18) << "Increment operator";
      cout << setw(18) << "# of increments";
      cout << setw(22) << "Difference (seconds)";
      cout << setw(21) << "Difference (meters)";
      cout << setw(23) << "Acceptable Diff (sec)";
      

      cout << endl;

      cout << setw(18) << "------------------";
      cout << setw(18) << "---------------";
      cout << setw(22) << "--------------------";
      cout << setw(21) << "-------------------";
      cout << setw(23) << "--------------------";
      cout << endl;

      TEST_METHOD(operator+=, 60,      1, 1e-15)
      TEST_METHOD(operator+=, 3600,    1, 1e-15)
      TEST_METHOD(operator+=, 86400,   1, 1e-15)
      TEST_METHOD(operator+=, 7*86400, 1, 1e-15)
      cout << endl;


      TEST_METHOD(addMilliSeconds, 1000,         1, 1e-15)
      TEST_METHOD(addMilliSeconds, 60*1000,      1, 1e-15)
      TEST_METHOD(addMilliSeconds, 3600*1000,    1, 1e-15) 
      TEST_METHOD(addMilliSeconds, 86400*1000,   1, 1e-15)
      TEST_METHOD(addMilliSeconds, 86400*2*1000, 1, 1e-15)
      TEST_METHOD(addMilliSeconds, 86400*7*1000, 1, 1e-15)
      cout << endl;
      
      TEST_METHOD(addMicroSeconds, 1000*1000,      1, 1e-3)
      TEST_METHOD(addMicroSeconds, 60*1000*1000,   1, 1e-3)
      TEST_METHOD(addMicroSeconds, 5*60*1000*1000, 1, 1e-3)
      cout << endl;
      
      DayTime endTime;

      
      cout << endl << setprecision(4);
      cout << endTime.printf("Completed on %B %d, %Y %H:%02M:%02S") << endl;
      cout << "Processing time " << endTime-startTime << " seconds." << endl;
      cout << endl;
      
      int ret = 0;
      if (cumulativeResult)
         cout << "All comparison tests PASSED." << endl;
      else
      {
         cout << "One ore more comparison tests FAILED." << endl;
         ret = 1;
      }

      cout << endl;
      
      return ret;
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
