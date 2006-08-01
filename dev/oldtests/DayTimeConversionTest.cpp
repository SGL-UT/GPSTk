#pragma ident "$Id$"


/* \example DayTimeToleranceTest.cpp
 * This test evaluates user defined comparison tolerance feature of DayTime.
 */

#include <iostream>
#include <stdlib.h>

#include "StringUtils.hpp"
#include "DayTime.hpp"

using namespace std;
using namespace gpstk;

bool testConstructors(short year, short month, short dom,
                      short hour, short minute, double seconds,
                      short week, double sow, long zcount,
                      short hintYear,
                      double MJD) 
{
   bool cumulativeResult = true;

   gpstk::DayTime case1(year,month,dom,
                        hour, minute, seconds); // Calendar constructor
   gpstk::DayTime case2(week%1024, sow, hintYear); // 10 bit GPS+SOW wk w/hint
   gpstk::DayTime case3(week%1024,zcount, hintYear); // 10 bit GPS+Z count w/hint
   gpstk::DayTime case4(MJD); // JD converted to MJD inline

   cout << case1.printf("  %-9B %02d, %Y %02H:%02M:%018.15f ");

   cumulativeResult = cumulativeResult && 
         (case1==case2) && (case1==case3) && (case1==case4);

   if (cumulativeResult)
      cout << "PASS" << endl;
   else
   {
      cout << " FAIL" << endl << "    Conversion results were: " << endl;
      cout << case1.printf("    %-10B %02d, %Y %02H:%02M:%02S   ") << endl;
      cout << case2.printf("    %-10B %02d, %Y %02H:%02M:%02S   ") << endl;
      cout << case3.printf("    %-10B %02d, %Y %02H:%02M:%02S   ") << endl;
      cout << case4.printf("    %-10B %02d, %Y %02H:%02M:%02S   ") << endl;
   }

   return (cumulativeResult);   
}


bool testMutators(short year, short month, short dom, 
                  short hour, short minute, double seconds,
                  short doy, double sod,
                  short week, double sow, long zcount,
                  short hintYear,
                  double MJD) 
{
   bool cumulativeResult = true;

   gpstk::DayTime case1, case2, case3, case4, case5, case6;
   
   case1.setYMD(year, month, dom);
   case1.setHMS(hour, minute, seconds);

   case2.setYMDHMS(year, month, dom, hour, minute, seconds);
   
   case3.setYDoy(year, doy);
   case3.setSecOfDay(sod);
   
   case4.setGPS(week%1024, sow, hintYear);

   case5.setGPS(week%1024, zcount, hintYear);

   case6.setMJD(MJD);
   
   cout << case1.printf("  %-9B %02d, %Y %02H:%02M:%018.15f ");
   
   cumulativeResult = cumulativeResult && 
         (case1==case2) && (case1==case3) && (case1==case4) &&
         (case1.GPSzcount()==case5.GPSzcount()) && 
         (fabs(case1.MJD()-case6.MJD())<.001/DayTime::SEC_DAY);

   if (cumulativeResult)
      cout << "PASS" << endl;
   else
   {
      cout << "FAIL" << endl << "    Conversion results were: " << endl;
      cout << case1.printf("      %-10B %02d, %Y %02H:%02M:%018.15f  using setYMD, setHMS ") << endl;
      cout << case2.printf("      %-10B %02d, %Y %02H:%02M:%018.15f  using setYMDHMS ") << endl;
      cout << case3.printf("      %-10B %02d, %Y %02H:%02M:%018.15f  using setYDoy, setSecOfDay ") << endl;
      cout << case4.printf("      %-10B %02d, %Y %02H:%02M:%018.15f  using setGPS(wk, sow, hintYr) ") << endl;
      cout << case5.printf("      %-10B %02d, %Y %02H:%02M:%018.15f  using setGPS(wk, zcount, hintYr) ") << endl;
      cout << case6.printf("      %-10B %02d, %Y %02H:%02M:%018.15f  using setMJD ") << endl;
   }

   return (cumulativeResult);   
}
   
bool testRandomAccessors(DayTime &dtb, DayTime &dte, long ndates)
{
   bool cumulativeResult=true;

      // Seed the random number generator
   gpstk::DayTime dt;
   unsigned int seed= (unsigned int) dt.GPSsow();
   srand(seed);

   unsigned long dayDiff = (unsigned long) 
                           ceil(dte.MJD() - dtb.MJD());
   
   for (int j=0;j<ndates;++j) // Loop through set of random days
   { 
      double dayDelta = floor( rand()*1./ RAND_MAX * dayDiff);
      double sodDelta = rand()*1./RAND_MAX;
      gpstk::DayTime testDate;

      testDate.setMJD(dtb.MJD()+dayDelta+sodDelta);
         
      short year =     testDate.year();
      short month =    testDate.month();
      short dom =      testDate.day();
      short hour =     testDate.hour();
      short minute =   testDate.minute();
      double seconds = testDate.second();
      short doy =      testDate.DOY();
      double sod =     testDate.DOYsecond();
      short week =     testDate.GPSfullweek()%1024; 
      double sow =     testDate.GPSsow();
      long zcount =    testDate.GPSzcount();
      short hintYear = testDate.year();
      double MJD =     testDate.MJD();
      cumulativeResult = cumulativeResult &&
         testMutators( year, month, dom, hour, minute, seconds,
                       doy, sod,
                       week, sow, zcount, hintYear,
                       MJD );
      } // End loop over random dates
   
   return cumulativeResult;
}

/// returns 0 if all tests pass
int main()
{
   using gpstk::DayTime;
   
   try
   {
      DayTime::setDayTimeTolerance(DayTime::DAYTIME_TOLERANCE); // microsecond tolerance

      cout << endl;
      cout << "DayTime conversion tests." << endl << endl;
      
      cout << "All comparisons accurate to " << DayTime::DAYTIME_TOLERANCE;
      cout << " seconds." << endl << endl;
      

      bool cumulativeResult = true;


      cout << "Testing constructors using documented dates." << endl;
      cout << endl;
      
      
         // Directly from ICD-GPS-200
         // Beginning of GPS Time, as defined by ICD-GPS-200
      cumulativeResult = cumulativeResult &&
                             testConstructors(1980,1,6,0,0,0,
                                              0, 0., 0,
                                              1981,
                                              44244.);
      
         // From GPS Signals and Performan, Misra and Enge, p. 91
         // GPS 10 bit week rollover epoch
      cumulativeResult = cumulativeResult &&
                             testConstructors(1999,8,22,0,0,0,
                                              0, 0., 0,
                                              2000,
                                              51412.);
      
         // From Hoffman-Wellenhof, et al. 
         // The J2000 standard epoch
      cumulativeResult = cumulativeResult &&
                             testConstructors(2000,1,1,12,0,0,
                                              1042, 561600., 374400,
                                              2000,
                                              2451545 - 2400000.5);


      cout << endl << "Testing mutators using documented dates." << endl << endl;
      
         // Directly from ICD-GPS-200
         // Beginning of GPS Time, as defined by ICD-GPS-200
      cumulativeResult = cumulativeResult &&
                             testMutators(1980,1,6,0,0,0,
                                          6, 0.,
                                          0, 0., 0,
                                          1981,
                                          44244.);
      
         // From GPS Signals and Performan, Misra and Enge, p. 91
         // GPS 10 bit week rollover epoch
      cumulativeResult = cumulativeResult &&
                             testMutators(1999,8,22,0,0,0,
                                          234, 0.,
                                          0, 0., 0,
                                          2000,
                                          51412.);
      
         // From Hoffman-Wellenhof, et al. 
         // The J2000 standard epoch
      cumulativeResult = cumulativeResult &&
                             testMutators(2000,1,1,12,0,0,
                                          1, 43200.,
                                          1042, 561600., 374400,
                                          2000,
                                          2451545 - 2400000.5);

         // Random accessor/mutator tests
      cout << endl;
      cout << "Testing accessors and mutators using randomly generated dates.";
      cout << endl << endl;
      
      gpstk::DayTime dtBegin(1995,1,1,0,0,0), dtEnd(2015,1,1,0,0,0);
      cumulativeResult = cumulativeResult &&
                         testRandomAccessors( dtBegin, dtEnd, 20);


         // Wrap it up, folks
      cout << endl;
      cout << setw(34);
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


