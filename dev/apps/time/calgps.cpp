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

#include "PSImage.hpp"
#include "EPSImage.hpp"
#include "SVGImage.hpp"
#include "Frame.hpp"
#include "HLayout.hpp"

using namespace std;
using namespace gpstk;
using namespace vplot;

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

void plotMonth(short month, short year, 
               const Frame& f)
{
   CivilTime ct(year, month, 1, 0, 0, 0.0);
   CommonTime thisDay(ct);
   GPSWeekSecond gws(thisDay);
   int gpsweek = gws.week;
   short dow   = static_cast<long>(gws.sow) / SEC_PER_DAY; 

   Color bgclr(220,220,220);
   Rectangle background(f.lx(),f.ly(),f.ux(),f.uy(),
                        StrokeStyle(Color(Color::CLEAR),0), bgclr);
   f << background;
   
   TextStyle ts((double)15.0, (int) TextStyle::BOLD, Color::BLACK, 
                TextStyle::SANSSERIF);
   f << Text(ct.printf("%b %4Y").c_str(),f.cx(),f.cy(),ts,Text::CENTER);

/*
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
*/
}

int main(int argc, char* argv[])
{

   try {
      
      CommandOptionNoArg helpOption('h',"help","Display argument list.",false);
      CommandOptionNoArg threeOption('3',"three-months","Display last, this and next months.",false);
      CommandOptionNoArg thisYearOption('y',"year","Display all months for the current year");
      CommandOptionWithNumberArg givenYearOption('Y',"specific-year","Display all months for a given year");
      CommandOptionWithAnyArg postscriptOption('p',"postscript","Generate a postscript file");
      CommandOptionWithAnyArg svgOption('s',"svg","Generate an SVG file");
      CommandOptionWithAnyArg epsOption('e',"eps","Generate an encapsulated postscript file");
      CommandOptionNoArg viewOption('v',"view","Try to launch an appropriate viewer for the file.");
      
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

         /**************graphic output setup***********************/ 
      bool drawCalendar=false;
      bool displayCalendar=(viewOption.getCount()>0);
      int nmonths=1;
      if (threeOption.getCount()) nmonths=3;
      if ((thisYearOption.getCount())||(givenYearOption.getCount())) nmonths=12;
      int nrows = nmonths;
      int ncols = 1;
      if (nmonths > 6)
      {
         nrows = 6;
         ncols = 2;
      }
      double mwidth = 4* 72; // 4 inches
      double mheight = 3 * 72; // 3 inches
      double twidth=mwidth*ncols;
      double theight=mheight*nrows;
      
      VGImage* vgs = 0; 
      Frame* frame = 0;
      HLayout* vlayout = 0;

      if (postscriptOption.getCount())
      {
         drawCalendar=true;
         vgs = new PSImage(postscriptOption.getValue()[0].c_str(),
                          twidth, theight, VGImage::UPPER_LEFT);
      }
      if (epsOption.getCount())
      {
         drawCalendar=true;
         vgs = new EPSImage(epsOption.getValue()[0].c_str(),
                             0,0,twidth, theight, VGImage::UPPER_LEFT);
      }
      if(svgOption.getCount())
      {
         drawCalendar=true;
         vgs = new SVGImage(svgOption.getValue()[0].c_str(),
                             twidth, theight, VGImage::UPPER_LEFT);
      }
      if (drawCalendar)
      {
         frame   = new Frame(*vgs);
         vlayout = new HLayout(*frame,nrows);
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

      int mcount=0;
      for (short m=firstMonth, y=firstYear;
          (y<lastYear) || ((m<=lastMonth) && (y==lastYear)); 
           m++)
      {
         if (m==13)
         {
            m=1;
            y++;
         }
         
         if (!drawCalendar)
            printMonth(m, y);

         if (drawCalendar)
         {
            plotMonth(m, y, vlayout->getFrame(mcount++));
         }
         
      }
  
      cout << endl;
      
      if ((displayCalendar) && (vgs!=0))
         vgs->view();

      if (vgs != 0)
      {
	delete vgs;
	delete frame;
	delete vlayout;
      }
      
   }
   catch( Exception error)
   {
      cout << error << endl;
      exit(-1);
   }

   exit(0);
}
