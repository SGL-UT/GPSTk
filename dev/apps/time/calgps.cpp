#pragma ident "$Id$"

#include <iostream>
#include <iomanip>
#include "CommonTime.hpp"
#include "CivilTime.hpp"
#include "GPSWeekSecond.hpp"
#include "TimeConstants.hpp"
#include "TimeString.hpp"
#include "SystemTime.hpp"
#include "CommandOptionParser.hpp"

using namespace std;
using namespace gpstk;

void printMonth(short month, short year)
{
   CivilTime ct(year, month, 1, 0, 0, 0.0);
   CommonTime thisDay(ct);
   GPSWeekSecond gws(thisDay);
   int gpsweek = gws.week;
   short dow   = static_cast<long>(gws.sow) / SEC_PER_DAY; 
  
   cout << endl << ct.printf("%26b %4Y") << endl;

   bool done = false;
   do 
   {
      cout << setw(4) << gpsweek << "  ";
      short thisDow=0;
      
      while (thisDow < 7)
      {
         thisDay = GPSWeekSecond(gpsweek, thisDow*SEC_PER_DAY+.01 ).convertToCommonTime();
         CivilTime ct(thisDay);
         int thisMonth = ct.month;  
         if (thisMonth==month)
	   cout << printTime(thisDay, "%2d-%03j ");
         else 
           cout << "       ";

         // Iterate
         thisDow++;        
      }
      cout << endl;

      gpsweek++;
      thisDay = GPSWeekSecond(gpsweek, 0.0).convertToCommonTime();
      CivilTime ct(thisDay);
    
      done = ( (ct.month > month) || 
               (ct.year  > year)     );
   } while (!done);
  
   return;
}

int main(int argc, char* argv[])
{

   try {
      
      CommandOptionNoArg helpOption('h',"help","Display argument list.",false);
      CommandOptionNoArg threeOption('3',"three-months","Display last, this and next months.",false);
      CommandOptionNoArg thisYearOption('y',"year","Display all months for the current year");
      CommandOptionWithNumberArg givenYearOption('Y',"specific-year","Display all months for a given year");
      CommandOptionParser cop("GPSTk GPS Calendar Generator");
      cop.parseOptions(argc, argv);

      if (cop.hasErrors())
      {
         cop.dumpErrors(cout);
         cop.displayUsage(cout);
         return 1;
      }

      if(helpOption.getCount())
      {
         cop.displayUsage(cout);
         return 0;
      }

         // Default condition is to just print this month
      SystemTime st;
      CivilTime now(st);
      int firstMonth = now.month;
      int lastMonth  = now.month;
      int firstYear  = now.year;
      int lastYear   = now.year;

      if (thisYearOption.getCount())
      {
         firstMonth =1;
         lastMonth  =12;
      }
      
      if (givenYearOption.getCount())
      {
         firstMonth =1;
         lastMonth  =12;
         
         firstYear = gpstk::StringUtils::asInt((givenYearOption.getValue())[0]);
         lastYear = firstYear;
         
      }

      if (threeOption.getCount())
      {
         firstMonth--;
         if (firstMonth==0)
         {
            firstMonth = 12;
            firstYear--;
         }
         
         lastMonth++;
         if (lastMonth==13)
         {
            lastMonth = 1;
            lastYear++;
         }
      }
      
      //cout << "first month " << firstMonth << " " << firstYear << endl;
      //cout << "last month " << lastMonth << " " << lastYear << endl;

      for (short m=firstMonth, y=firstYear;
          (y<lastYear) || ((m<=lastMonth) && (y==lastYear)); 
           m++)
      {
         if (m==13)
         {
            m=1;
            y++;
         }
         
         printMonth(m, y);
      }
  
      cout << endl;
      
   }
   catch( Exception error)
   {
      cout << error << endl;
      exit(-1);
   }

   exit(0);
}
