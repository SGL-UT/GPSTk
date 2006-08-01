#pragma ident "$Id$"


/* \example DayTimeToleranceTest.cpp
 * This test evaluates user defined comparison tolerance feature of DayTime.
 */

#include <iostream>

#include "StringUtils.hpp"
#include "DayTime.hpp"

using namespace std;

/// returns 0 if all tests pass
int main()
{
   using gpstk::DayTime;
   
   try
   {
      cout << "Testing DayTime tolerances." << endl;      

         // Set the DayTimes using Year, Month, Day, Hour, Minute, Second.
      gpstk::DayTime dt1(2000,12,1,0,0,0.0), 
                     dt2(2000,12,1,0,0,0.0);
    
      int nCases = 11;
      double secDiff[]=
        { 1.01,  1.0,  .99,   .5,  .25,   .0, -.25,  -.5, -.99, -1.0, - 1.01 };

      bool   oneSecTolPass[]=
        { false, true, true, true, true, true, true, true, true, true, false};
      
      bool   halfSecTolPass[]=
        { false, false, false, true, true, true, true, true, false, false, false};
         // column widths: 24 24 5 7 7 7 7
      cout << "         Time 1                   Time 2         "
         "  diff       1 sec         0.5 sec" << endl;

      string equal(  "   Equal");
      string inequal(" Inequal");

      bool cumulativeResult = true;
      
      for (int i=0; i<nCases; i++)
      {
         dt2 = dt1 + secDiff[i];
         string fmt("%m/%d/%Y %02H:%02M:%07.4f");
         cout << setw(24) << dt1.printf(fmt) << " " 
              << setw(24) << dt2.printf(fmt) << " " 
              << setw(5) << dt2 - dt1 << " ";

            // Set dt1 and dt2 to one second tolerance.
         dt1.setTolerance(gpstk::DayTime::ONE_SEC_TOLERANCE);
         dt2.setTolerance(gpstk::DayTime::ONE_SEC_TOLERANCE);
         
         cout << (oneSecTolPass[i] ? equal : inequal)
              << ((dt1 == dt2)     ? equal : inequal) ;

         cumulativeResult = cumulativeResult && (oneSecTolPass[i]==(dt1==dt2));
         
            // Set dt1 and dt2 to half second tolerance.
         dt1.setTolerance(gpstk::DayTime::ONE_SEC_TOLERANCE * 0.5);
         dt2.setTolerance(gpstk::DayTime::ONE_SEC_TOLERANCE * 0.5);

         cout << (halfSecTolPass[i]  ? equal : inequal) 
              << ((dt1 == dt2)       ? equal : inequal)
              << endl ;

         cumulativeResult = cumulativeResult && 
                            (halfSecTolPass[i]==(dt1==dt2));

      }

      cout << endl;
      cout << DayTime().printf("Completed on %B %d, %Y %H:%02M:%02S");
      cout << endl << endl;
      
      int ret = 0;
      if (cumulativeResult)
         cout << "All comparison tests PASSED." << endl;
      else
      {
         cout << "One ore more comparison tests FAILED." << endl;
         ret = 1;
      }
      
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
