#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/time/calgps.cpp#1 $"

// FIXME. This calendar app should simplify by updating it 
//        to use the new TimeTag class. Not sure which one to user now tho..

#include <iostream>
#include <iomanip>
#include "DayTime.hpp"
#include "CommandOptionParser.hpp"

using namespace std;
using namespace gpstk;

void printMonth(short month, short year)
{
   DayTime thisDay(year, month, 1, 0, 0, 0.0);

   short gpsweek = thisDay.GPSfullweek();
   short dow     = thisDay.GPSday();
  
   cout << endl << thisDay.printf("%26b %4Y") << endl;

   bool done = false;
   do 
   {
      cout << setw(4) << gpsweek << "  ";
      short thisDow=0;
      
      while (thisDow < 7)
      {
         thisDay = DayTime(gpsweek, thisDow* gpstk::DayTime::SEC_DAY);
         short thisMonth = thisDay.month();  
         if (thisMonth==month)
	   cout << thisDay.printf("%2d-%03j ");
         else 
           cout << "       ";

         // Iterate
         thisDow++;        
      }
      cout << endl;

      gpsweek++;
      thisDay = DayTime(gpsweek, 0.0);
      
      done = ( (thisDay.month()>month) || 
               (thisDay.year() >year)     );
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
      DayTime now;
      short firstMonth = now.month();
      short lastMonth  = now.month();
      short firstYear  = now.year();
      short lastYear   = now.year();

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
      

      for (short m=firstMonth, y=firstYear;
          (m<=lastMonth) && (y<=lastYear); 
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
