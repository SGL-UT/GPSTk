//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

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

using namespace std;
using namespace gpstk;

void printMonth(short month, short year)
{
   CivilTime civ(year, month, 1, 0, 0, 0.0);
  
   cout << endl << civ.printf("%26b %4Y") << endl;
   
   GPSWeekSecond gws(civ);

   for (; civ.month == month; ++gws.week, gws.sow = 0, civ = gws)
   {
      cout << setw(4) << gws.week << "  ";
      
      for (short thisDow = 0; thisDow < 7; ++thisDow)
      {
         gws.sow = thisDow * SEC_PER_DAY;
         CommonTime com(gws);
         if (CivilTime(com).month == month)
	   cout << printTime(com, "%2d-%03j ");
         else 
           cout << "       ";
      }
      cout << endl;
   }
}

int main(int argc, char* argv[])
{

   try {
      
      CommandOptionNoArg helpOption('h',"help","Display argument list.",false);
      CommandOptionNoArg threeOption('3',"three-months","Display last, this and next months.",false);
      CommandOptionNoArg thisYearOption('y',"year","Display all months for the current year");
      CommandOptionWithNumberArg givenYearOption('Y',"specific-year","Display all months for a given year");
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
    
      
      // Print this month
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
