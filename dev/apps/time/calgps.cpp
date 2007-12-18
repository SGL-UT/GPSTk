#pragma ident "$Id$"

#include <iostream>
#include <iomanip>
#include "CommonTime.hpp"
#include "CivilTime.hpp"
#include "YDSTime.hpp"
#include "GPSWeekSecond.hpp"
#include "TimeConstants.hpp"
#include "TimeString.hpp"
#include "SystemTime.hpp"
#include "CommandOptionParser.hpp"
#include "StringUtils.hpp"

#include "PSImage.hpp"
#include "EPSImage.hpp"
#include "SVGImage.hpp"
#include "Frame.hpp"
#include "VLayout.hpp"
#include "HLayout.hpp"
#include "GridLayout.hpp"
#include "BorderLayout.hpp"

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
               const Frame& outerFrame, double monthBorderSize=0,
               double printYear=true)
{         
      // Create the border/margin
   BorderLayout marginLayout(outerFrame, monthBorderSize);
   Frame mframe=marginLayout.getFrame(0);
   
   VLayout titlebodyLayout(mframe, .20);
   Frame ft = titlebodyLayout.getFrame(0);
   Frame fb = titlebodyLayout.getFrame(1);
   HLayout weekDOWLayout(fb,0.2);
   
   CivilTime firstDOM(year, month, 1, 0, 0, 0.0);
   CivilTime nextFirstDOM;
   if (month==12) nextFirstDOM = CivilTime(year+1, 1, 1, 0, 0, 0.0);
   else nextFirstDOM = CivilTime(year, month+1, 1, 0, 0, 0.0);
   CommonTime lastDOM = CommonTime(nextFirstDOM) - SEC_PER_DAY+1;

   CivilTime thisDay(firstDOM);
   GPSWeekSecond gws(thisDay);
   int gpsweek1 = gws.week;
   
   short dow1   = static_cast<long>(gws.sow) / SEC_PER_DAY; 

   CivilTime thatDay(lastDOM);
   GPSWeekSecond gws2(thatDay);
   int gpsweek2 = gws2.week;
   short dow2   = static_cast<long>(gws.sow) / SEC_PER_DAY;

   TextStyle ts(ft.getHeight()*.5, (int) TextStyle::BOLD, Color::BLACK, 
                TextStyle::SANSSERIF);
   if (printYear)
   {
      ft << Text(thisDay.printf("%b %4Y").c_str(),
                 ft.cx(),ft.uy()-.25*ft.getHeight(),
                 ts,Text::CENTER);
   }
   else
   {
      ft << Text(thisDay.printf("%B").c_str(),
                 ft.cx(),ft.uy()-.25*ft.getHeight(),
                 ts,Text::CENTER);
   }

   VLayout weekListLayout(weekDOWLayout.getFrame(0),gpsweek2-gpsweek1+1);
   VLayout weekListLayoutRows(fb,gpsweek2-gpsweek1+1);

   Color bgclr(230,230,230);

   double wkHeight=weekListLayoutRows.getFrame(0).getHeight()*.4;
   TextStyle tsw(wkHeight, (int) TextStyle::BOLD, Color::BLACK, 
                TextStyle::SERIF);
   for (int week=gpsweek1; week<=gpsweek2; week++)
   {
      if (week%2 == 1)
      {
         Frame fwkrow = weekListLayoutRows.getFrame(week-gpsweek1);
         Rectangle background(fwkrow.lx(),fwkrow.ly(),
                              fwkrow.ux(),fwkrow.uy(),
                              StrokeStyle(bgclr,0), bgclr);
         fwkrow << background;
      }  
      Frame ftemp = weekListLayout.getFrame(week-gpsweek1);
      ftemp << Text(week,ftemp.cx(), ftemp.uy()-.3*ftemp.getHeight(), tsw, Text::CENTER);

   }

   GridLayout domLayout(weekDOWLayout.getFrame(1),gpsweek2-gpsweek1+1,7);

   TextStyle tsdom(domLayout.getFrame(0).getHeight()*.40,
                   (int) TextStyle::BOLD, Color::BLACK, 
                   TextStyle::SERIF);
   double doyHt=domLayout.getFrame(0).getHeight()*.27;
   TextStyle tsdoy(doyHt,
                   (int) TextStyle::BOLD, Color::BLUE, 
                   TextStyle::SERIF);

   while (thisDay.day <= thatDay.day)
   {
      GPSWeekSecond gwsTemp(thisDay);
      int gpsweek = gwsTemp.week;
      int dow   = static_cast<long>(gwsTemp.sow) / SEC_PER_DAY;

      YDSTime ydsTemp(thisDay);
      int doy = ydsTemp.doy;

      Frame ftemp = domLayout.getFrame(gpsweek-gpsweek1, dow);
      ftemp << Text(thisDay.day,ftemp.cx(), ftemp.uy()-.5*ftemp.getHeight(), 
                    tsdom, Text::CENTER);
      ftemp << Text(doy, ftemp.ux(), ftemp.uy()-.1*ftemp.getHeight(), 
                    tsdoy, Text::RIGHT);

      thisDay.day++;
   }
   
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
      CommandOptionNoArg blurbOption('n',"no-blurb","Suppress GPSTk reference in graphic output.");
      
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

      // Default case of nmonths = 1
      int nrows = 1;
      int ncols = 1;
      double pageMargin = PTS_PER_INCH / 3; // Outer margin in points
      double monthBorder = PTS_PER_INCH / 10;
      double mwidth = 4*PTS_PER_INCH; 
      double mheight = 3*PTS_PER_INCH; 
      double twidth = mwidth;
      double theight = mheight;
      
      if (nmonths==3)
      {
         nrows = 3;
         ncols = 1;
         pageMargin = PTS_PER_INCH / 3; 
         monthBorder = PTS_PER_INCH / 10;
         mwidth = 4*PTS_PER_INCH; 
         mheight = 3*PTS_PER_INCH; 
         twidth = mwidth + 2*pageMargin;
         theight = mheight*nrows + 2*pageMargin;
      }
         
      if (nmonths==12)
      {
         nrows = 4;
         ncols = 3;
            // Don't know how to detect when to set this equal to metric
            // values (e.g,  A4_WIDTH_PTS)
        twidth = US_LETTER_WIDTH_PTS;
        theight = US_LETTER_HEIGHT_PTS;
        pageMargin = PTS_PER_INCH / 3; 
        monthBorder = PTS_PER_INCH / 8;
        mwidth = (twidth - 2*pageMargin) / ncols;
        mheight = (theight - 2 *pageMargin) / nrows;
      }
      
      VGImage* vgs = 0; 
      Frame* frame = 0;
      Layout* layout = 0;

      if (postscriptOption.getCount())
      {
         drawCalendar=true;
         vgs = new PSImage(postscriptOption.getValue()[0].c_str(),
                           twidth, theight, 
                           VGImage::UPPER_LEFT);
      }
      if (epsOption.getCount())
      {
         drawCalendar=true;
         vgs = new EPSImage(epsOption.getValue()[0].c_str(),
                            0,0,twidth,theight, 
                            VGImage::UPPER_LEFT);
      }
      if(svgOption.getCount())
      {
         drawCalendar=true;
         vgs = new SVGImage(svgOption.getValue()[0].c_str(),
                             twidth, theight, VGImage::UPPER_LEFT);
      }
      if (drawCalendar)
      {
         Frame frameOutermost(*vgs);


            // Fill the frame with white, clear the area so to speak
         Rectangle backfill(frameOutermost.lx(), frameOutermost.ly(),
                            frameOutermost.ux(),frameOutermost.uy(), Color::WHITE);
         frameOutermost << backfill;


         BorderLayout bordersLayout(frameOutermost,pageMargin);
         frame = new Frame(bordersLayout.getFrame(0));

         if (ncols==1)
            layout = new VLayout(*frame,nrows);
         if (ncols==3)
            layout = new GridLayout(*frame, nrows, ncols);
         if (nmonths==12)         
         {
            VLayout titleLayout(*frame,.1);
            TextStyle titleStyle(titleLayout.getFrame(0).getHeight()*.4,
                       TextStyle::BOLD, Color::BLACK,
                       TextStyle::SANSSERIF);
            Frame fttl = titleLayout.getFrame(0);
            if (givenYearOption.getCount())
            {
               fttl << Text((string("GPS Calendar ")+givenYearOption.getValue()[0]).c_str(), 
                            fttl.cx(), fttl.uy()-.3*fttl.getHeight(),
                            titleStyle, Text::CENTER);
            }
            else
            {
               SystemTime st;
               CivilTime now(st);
               fttl << Text((string("GPS Calendar ")+StringUtils::asString(now.year)).c_str(),
                            fttl.cx(), fttl.uy()-.25*fttl.getHeight(),
                            titleStyle, Text::CENTER);

            }

            if (blurbOption.getCount()==0)
            {
               VLayout blurbLayout(titleLayout.getFrame(1),0.95);
               TextStyle blurbStyle(blurbLayout.getFrame(1).getHeight()*.4,
                       TextStyle::NORMAL, Color::BLACK,
                       TextStyle::SERIF);
               TextStyle blurbStyleHttp(blurbLayout.getFrame(1).getHeight()*.4,
                       TextStyle::ITALIC, Color::BLACK,
                       TextStyle::SERIF);
               
               VLayout blurbStackLayout(blurbLayout.getFrame(1),0.6);
               Frame blurbFrame;

               blurbFrame = blurbStackLayout.getFrame(0);
               blurbFrame << Text("Generated using the GPS Toolkit", blurbFrame.cx(), blurbFrame.uy()-0.3*blurbFrame.getHeight() , blurbStyle, Text::CENTER);

               blurbFrame = blurbStackLayout.getFrame(1);
               blurbFrame << Text("http://www.gpstk.org/", blurbFrame.cx(), blurbFrame.uy()-0.3*blurbFrame.getHeight() , blurbStyleHttp, Text::CENTER);

               layout = new GridLayout(blurbLayout.getFrame(0),nrows,ncols);
            }
            else
            {
               layout = new GridLayout(titleLayout.getFrame(1),nrows,ncols);
            }
            
         }
         
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
            plotMonth(m, y, layout->getFrame(mcount++), monthBorder, (nmonths!=12));
         }
         
      }
  
      cout << endl;
      
      if ((displayCalendar) && (vgs!=0))
         vgs->view();

      if (vgs != 0)
      {
	delete vgs;
	delete frame;
	delete layout;
      }
      
   }
   catch( Exception error)
   {
      cout << error << endl;
      exit(-1);
   }

   exit(0);
}
