#include <iostream>

#include "CommonTime.hpp"
#include "TimeSystem.hpp"
#include "Exception.hpp"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

using namespace std;
using namespace gpstk;

bool addDaysTest();
bool addMillisecondsTest();
bool addSecondsDoubleTest();
bool addSecondsLongTest();

void parseArg(char* str);
void helpMessage();
void licence();

const char* name;
const char* author = "Troy Varney";
const char* date = "7/23/09";
const char* seperator = "-----------------------------------------------------"
                        "---------------------------";
const char* equalSep = "======================================================"
                       "==========================";
bool printSeperator;
bool silent;
bool bashColor;
bool daysTest;
bool msTest;
bool secDoubTest;
bool secLongTest;
bool all;

const char* bashFailColor = "\e[1;31m";
const char* bashPassColor = "\e[1;32m";
const char* bashNormalColor = "\e[0m";

int main(int argc, char** argv)
{
   //The name should be gaurenteed to be there.
   name = argv[0];
   
   printSeperator = false;
   silent = false;
   bashColor = false;
   
   all = true;
   daysTest = false;
   msTest = false;
   secDoubTest = false;
   secLongTest = false;
   
   if(argc > 1)
   {
      for(int i = 1; i < argc; ++i)
      {
         parseArg(argv[i]);
      }
   }
   
   int ret = 0;
   if(all || daysTest)
   {
      if(!addDaysTest())
      {
         cout << "addDays(long) Test ";
         if(bashColor)
         	cout << bashFailColor;
         cout << "failed";
         if(bashColor)
         	cout << bashNormalColor;
         cout << "..." << endl << endl;
         ret++;
      }
      else
		{
         cout << "addDays(long) Test ";
         if(bashColor)
         	cout << bashPassColor;
         cout << "passed";
         if(bashColor)
         	cout << bashNormalColor;
         cout << "..." << endl << endl;
      }
   }
   
   if(all || msTest)
   {
      if(!addMillisecondsTest())
      {
         cout << "addMilliseconds(long) Test ";
         if(bashColor)
         	cout << bashFailColor;
         cout << "failed";
         if(bashColor)
         	cout << bashNormalColor;
         cout << "..." << endl << endl;
         ret++;
      }
      else
      {
         cout << "addMilliseconds(long) Test ";
         if(bashColor)
         	cout << bashPassColor;
         cout << "passed";
         if(bashColor)
         	cout << bashNormalColor;
         cout << "..." << endl << endl;
		}
   }
   
   if(all || secDoubTest)
   {
      if(!addSecondsDoubleTest())
      {
         cout << "addSeconds(double) Test ";
         if(bashColor)
         	cout << bashFailColor;
         cout << "failed";
         if(bashColor)
         	cout << bashNormalColor;
         cout << "..." << endl << endl;
         ret++;
      }
      else
      {
         cout << "addSeconds(double) Test ";
         if(bashColor)
         	cout << bashPassColor;
         cout << "passed";
         if(bashColor)
         	cout << bashNormalColor;
         cout << "..." << endl << endl;
		}
   }
   
   if(all || secLongTest)
   {
      if(!addSecondsLongTest())
      {
         cout << "addSeconds(long) Test ";
         if(bashColor)
         	cout << bashFailColor;
         cout << "failed";
         if(bashColor)
         	cout << bashNormalColor;
         cout << "..." << endl << endl;
         ret++;
      }
      else
      {
         cout << "addSeconds(long) Test ";
         if(bashColor)
         	cout << bashPassColor;
         cout << "passed";
         if(bashColor)
         	cout << bashNormalColor;
         cout << "..." << endl << endl;
		}
   }
   
   return ret;
}

void parseArg(char* str)
{
   std::string arg = str;
   
   if(arg == "-h" || arg == "--help")
   {
      helpMessage();
   }
   else if(arg == "-l" || arg == "--licence")
   {
      licence();
   }
   else if(arg == "-s" || arg == "--silent")
   {
      silent = true;
   }
   else if(arg == "-f" || arg == "--fancy")
   {
      printSeperator = true;
   }
   else if(arg == "-a" || arg == "--all")
   {
   	all = true;
	}
	else if(arg == "-dt" || arg == "--daystest")
	{
		all = false;
		daysTest = true;
	}
	else if(arg == "-mst" || arg == "--mstest")
	{
		all = false;
		msTest = true;
	}
	else if(arg == "-sdt" || arg == "--secdoubtest")
	{
		all = false;
		secDoubTest = true;
	}
	else if(arg == "-slt" || arg == "--seclongtest")
	{
		all = false;
		secLongTest = true;
	}
	else if(arg == "-c" || arg == "--bashcolor")
	{
		bashColor = true;
	}
	
	else
	{
		cout << "Unknown option: " << arg << endl;
		helpMessage();
	}
}
void helpMessage()
{
	cout << endl;
   cout << "Usage: " << name << " [options][tests]" << endl;
   cout << "Options:" << endl;
   cout << "   -c | --bashcolor = Attempts to use bash color escapes to color output." << endl;
   cout << "   -f | --fancy   = Prints seperators and headers. Requires a term"
            " width of >80." << endl;
   cout << "   -h | --help    = Prints this message and quits." << endl;
   cout << "   -l | --licence = Prints the license information and quits."
        << endl;
   cout << "   -s | --silent  = Prints only pass/fail messages." << endl;
   
   cout << "Tests:" << endl;
   cout << "   -a   | --all         = Every test is run." << endl;
   cout << "   -dt  | --daystest    = The addDays() Test is run." << endl;
   cout << "   -mst | --mstest      = The addMilliseconds() Test is run."
        << endl;
   cout << "   -sdt | --secdoubtest = The addSeconds(double) test is run."
        << endl;
   cout << "   -slt | --seclongtest = The addSeconds(long) test is run."
        << endl;
   
   cout << endl;
   cout << "Running this program with no arguments is the same as:" << endl;
   cout << "   " << name << " --all" << endl;
   cout << "Default options are:" << endl;
   cout << "   silent? no" << endl;
   cout << "   fancy? no" << endl;
   cout << "This program returns 0 for success, or a number indicating the "
           "number of test" << endl << "failures." << endl << endl;
   
   exit(0);
}
void licence()
{
	cout << endl << author << endl << date << endl;
   cout << endl;
   cout << "This file is part of GPSTk, the GPS Toolkit." << endl;
   cout << endl;
   cout << "The GPSTk is free software; you can redistribute it and/or modify "
           "it under the" << endl;
   cout << "terms of the GNU Lesser General Public License as published by the"
           " Free Software" << endl;
   cout << "Foundation; either version 2.1 of the License, or any later "
           "version." << endl;
   cout << endl;
   cout << "The GPSTk is distributed in the hope that it will be useful, but "
           "WITHOUT ANY" << endl;
   cout << "WARRANTY; without even the implied warranty of MERCHANTABILITY or "
           "FITNESS FOR A" << endl;
   cout << "PARTICULAR PURPOSE.  See the GNU Lesser General Public License for"
           " more details." << endl;
   cout << endl;
   cout << "You should have received a copy of the GNU Lesser General Public "
           "License along" << endl;
   cout << "with GPSTk; if not, write to the Free Software Foundation, Inc., "
           "59 Temple" << endl;
   cout << "Place, Suite 330, Boston, MA  02111-1307  USA" << endl;
   cout << endl;
   cout << "Copyright 2004, The University of Texas at Austin" << endl;
   cout << endl;
   
   exit(0);
}

/*
 * Add a day to a time just before midnight
 * Add a millisecond to a time just before midnight so it goes past midnight
 * Add a fractional millisecond to a time just before midnight so it goes past midnight
 * Subtract a day, millisecond and fractional millisecond from a time just after midnight, separately so this is 3 tests.
 * Add seconds using the addSeconds(double) method such that the double is larger than SEC_PER_DAY
 * Add seconds using the addSeconds(double) method such that the double is larger then SEC_PER_MS
 * Add seconds using the addSeconds(long) method such that the long is larger than SEC_PER_DAY
 * Call the add method such that m_fsod is larger than SEC_PER_MS
 * Call the add method such that m_msod is larger than MS_PER_DAY
 * Call the add method such that m_fsod is less than zero
 * Call the add method such that m_msod is less than zero 
 */


bool addDaysTest()
{
   if(printSeperator)
   {
      cout << equalSep << endl;
      cout << "===                               addDaysTest()                "
           << "              ===" << endl << equalSep << endl;
   }
   else
   {
      cout << "addDaysTest()" << endl;
   }
   
   std::numeric_limits<double> limits;
   
   bool status = true;
   
   const long SEC_BEFORE_MIDNIGHT = SEC_PER_DAY - 1;
   const long MS_BEFORE_MIDNIGHT = MS_PER_DAY - 1;
   const double BEFORE_FSOD_LIM = SEC_PER_MS - 2 * limits.epsilon();
   
   CommonTime ctime;
   CommonTime oldct;
   
///-------------------------------------------------------------------------///
/// Days - before midnight
   ctime.set(1337L,SEC_BEFORE_MIDNIGHT,0);
   oldct = ctime;
   cout << "Start Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 1 days, 0 msod, 0 fsod" << endl;
   }
   ctime = ctime.addDays(1);
   cout << "Changed Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: -1 days, 0 msod, 0 fsod" << endl;
   }
   ctime = ctime.addDays(-1);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
   	if(bashColor)
   		cout << bashPassColor;
      cout << "---Test Passed---";
      if(bashColor)
      	cout << bashNormalColor;
   }
   else
   {
   	if(bashColor)
   		cout << bashFailColor;
      cout << "---Test Failed---";
      if(bashColor)
      	cout << bashNormalColor;
      status = false;
   }
   cout << endl << endl;
   if(printSeperator)
   {
      cout << seperator << endl;
   }
///-------------------------------------------------------------------------///
/// Days - after midnight
   ctime.set(1337L,0,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: -1 days, 0 msod, 0 fsod" << endl;
   }
   ctime = ctime.addDays(-1);
   cout << "Changed Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 1 days, 0 msod, 0 fsod" << endl;
   }
   ctime = ctime.addDays(1);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      if(bashColor)
   		cout << bashPassColor;
      if(bashColor)
   		cout << bashPassColor;
      cout << "---Test Passed---";
      if(bashColor)
      	cout << bashNormalColor;
      if(bashColor)
      	cout << bashNormalColor;
   }
   else
   {
      if(bashColor)
   		cout << bashFailColor;
      cout << "---Test Failed---";
      if(bashColor)
      	cout << bashNormalColor;
      status = false;
   }
   cout << endl << endl;
   if(printSeperator)
   {
      cout << seperator << endl;
   }
///-------------------------------------------------------------------------///
   
   return status;
}

//ignore for now
bool addMillisecondsTest()
{
   if(printSeperator)
   {
      cout << equalSep << endl;
      cout << "===                          addMillisecondsTest()             "
           << "              ===" << endl << equalSep << endl;
   }
   else
   {
      cout << "addMillisecondsTest()" << endl;
   }
   
   std::numeric_limits<double> limits;
   
   bool status = true;
   
   const long SEC_BEFORE_MIDNIGHT = SEC_PER_DAY - 1;
   const long MS_BEFORE_MIDNIGHT = MS_PER_DAY - 1;
   const double BEFORE_FSOD_LIM = SEC_PER_MS - 2 * limits.epsilon();
   
   CommonTime ctime;
   CommonTime oldct;
   
   ///-------------------------------------------------------------------------///
/// Milliseconds - before midnight
   ctime.set(1337L,SEC_BEFORE_MIDNIGHT,0);
   ctime.addMilliseconds(999);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, 1 msod, 0 fsod" << endl;
   }
   ctime = ctime.addMilliseconds(1);
   cout << "Changed Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, -1 msod, 0 fsod" << endl;
   }
   ctime = ctime.addMilliseconds(-1);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      if(bashColor)
   		cout << bashPassColor;
      cout << "---Test Passed---";
      if(bashColor)
      	cout << bashNormalColor;
   }
   else
   {
      if(bashColor)
   		cout << bashFailColor;
      cout << "---Test Failed---";
      if(bashColor)
      	cout << bashNormalColor;
      status = false;
   }
   cout << endl << endl;
   if(printSeperator)
   {
      cout << seperator << endl;
   }
///-------------------------------------------------------------------------///
/// Milliseconds - after midnight
   ctime.set(1337L,0,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, -1 msod, 0 fsod" << endl;
   }
   ctime = ctime.addMilliseconds(-1);
   cout << "Changed Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, 1 msod, 0 fsod" << endl;
   }
   ctime = ctime.addMilliseconds(1);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      if(bashColor)
   		cout << bashPassColor;
      cout << "---Test Passed---";
      if(bashColor)
      	cout << bashNormalColor;
   }
   else
   {
      if(bashColor)
   		cout << bashFailColor;
      cout << "---Test Failed---";
      if(bashColor)
      	cout << bashNormalColor;
      status = false;
   }
   cout << endl << endl;
   if(printSeperator)
   {
      cout  << seperator << endl;
   }
///-------------------------------------------------------------------------///
/// Milliseconds - midday +
   ctime.set(1337L,SEC_BEFORE_MIDNIGHT/2,0);
   oldct = ctime;
   cout << "Start Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, 1 msod, 0 fsod" << endl;
   }
   ctime = ctime.addMilliseconds(1);
   cout << "Changed Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, -1 msod, 0 fsod" << endl;
   }
   ctime = ctime.addMilliseconds(-1);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      if(bashColor)
   		cout << bashPassColor;
      cout << "---Test Passed---";
      if(bashColor)
      	cout << bashNormalColor;
   }
   else
   {
      if(bashColor)
   		cout << bashFailColor;
      cout << "---Test Failed---";
      if(bashColor)
      	cout << bashNormalColor;
      status = false;
   }
   cout << endl << endl;
   if(printSeperator)
   {
      cout << seperator << endl;
   }
///-------------------------------------------------------------------------///
/// Milliseconds - midday -
   ctime.set(1337L,SEC_BEFORE_MIDNIGHT/2,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, -1 msod, 0 fsod" << endl;
   }
   ctime = ctime.addMilliseconds(-1);
   cout << "Changed Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, 1 msod, 0 fsod" << endl;
   }
   ctime = ctime.addMilliseconds(1);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      if(bashColor)
   		cout << bashPassColor;
      cout << "---Test Passed---";
      if(bashColor)
      	cout << bashNormalColor;
   }
   else
   {
      if(bashColor)
   		cout << bashFailColor;
      cout << "---Test Failed---";
      if(bashColor)
      	cout << bashNormalColor;
      status = false;
   }
   cout << endl << endl;
   if(printSeperator)
   {
      cout << seperator << endl;
   }
   
   return status;
}

bool addSecondsDoubleTest()
{
   if(printSeperator)
   {
      cout << equalSep << endl;
      cout << "===                          addSecondsDoubleTest()               "
           << "           ===" << endl << equalSep << endl;
   }
   else
   {
      cout << "addSecondsDoubleTest()" << endl;
   }
   
   std::numeric_limits<double> limits;
   
   bool status = true;
   
   const long SEC_BEFORE_MIDNIGHT = SEC_PER_DAY - 1;
   const long MS_BEFORE_MIDNIGHT = MS_PER_DAY - 1;
   const double BEFORE_FSOD_LIM = SEC_PER_MS - 2 * limits.epsilon();
   
   CommonTime ctime(1337L, SEC_BEFORE_MIDNIGHT, BEFORE_FSOD_LIM);
   CommonTime oldct(ctime);
   
///-------------------------------------------------------------------------///
///Seconds - before midnight
   
   cout << "Start Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, 1500 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(1.5);
   cout << "Changed Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, -1500 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(-1.5);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      if(bashColor)
   		cout << bashPassColor;
      cout << "---Test Passed---";
      if(bashColor)
      	cout << bashNormalColor;
   }
   else
   {
      if(bashColor)
   		cout << bashFailColor;
      cout << "---Test Failed---";
      if(bashColor)
      	cout << bashNormalColor;
      status = false;
   }
   cout << endl << endl;
   if(printSeperator)
   {
      cout << seperator << endl;
   }
///-------------------------------------------------------------------------///
///Seconds - After midnight
   ctime.set(1337L,0,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, -1500 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(-1.5);
   cout << "Changed Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, 1500 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(1.5);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      if(bashColor)
   		cout << bashPassColor;
      cout << "---Test Passed---";
      if(bashColor)
      	cout << bashNormalColor;
   }
   else
   {
      if(bashColor)
   		cout << bashFailColor;
      cout << "---Test Failed---";
      if(bashColor)
      	cout << bashNormalColor;
      status = false;
   }
   cout << endl << endl;
   if(printSeperator)
   {
      cout << seperator << endl;
   }
   
///-------------------------------------------------------------------------///
///Seconds - Noon +
   ctime.set(1337L,SEC_BEFORE_MIDNIGHT/2,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, 1500 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(1.5);
   cout << "Changed Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, -1500 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(-1.5);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      if(bashColor)
   		cout << bashPassColor;
      cout << "---Test Passed---";
      if(bashColor)
      	cout << bashNormalColor;
   }
   else
   {
      if(bashColor)
   		cout << bashFailColor;
      cout << "---Test Failed---";
      if(bashColor)
      	cout << bashNormalColor;
      status = false;
   }
   cout << endl << endl;
   if(printSeperator)
   {
      cout << seperator << endl;
   }
   
///-------------------------------------------------------------------------///
///Seconds - Noon -
   ctime.set(1337L,SEC_BEFORE_MIDNIGHT/2,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, -1500 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(-1.5);
   cout << "Changed Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, 1500 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(1.5);
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      if(bashColor)
   		cout << bashPassColor;
      cout << "---Test Passed---";
      if(bashColor)
      	cout << bashNormalColor;
   }
   else
   {
      if(bashColor)
   		cout << bashFailColor;
      cout << "---Test Failed---";
      if(bashColor)
      	cout << bashNormalColor;
      status = false;
   }
   cout << endl << endl;
   if(printSeperator)
   {
      cout << seperator << endl;
   }
///-------------------------------------------------------------------------///


   return status;
}

bool addSecondsLongTest()
{
   if(printSeperator)
   {
      cout << equalSep << endl;
      cout << "===                           addSecondsLongTest()                "
           << "           ===" << endl << equalSep << endl;
   }
   else
   {
      cout << "addSecondsLongTest()" << endl;
   }
   
   std::numeric_limits<double> limits;
   
   bool status = true;
   
   const long SEC_BEFORE_MIDNIGHT = SEC_PER_DAY - 1;
   const long MS_BEFORE_MIDNIGHT = MS_PER_DAY - 1;
   const double BEFORE_FSOD_LIM = SEC_PER_MS - 2 * limits.epsilon();
   
   CommonTime ctime(1337L, SEC_BEFORE_MIDNIGHT, BEFORE_FSOD_LIM);
   CommonTime oldct(ctime);
///-------------------------------------------------------------------------///
///Seconds - before midnight
   
   cout << "Start Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, 1000 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(static_cast<long int>(1));
   cout << "Changed Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, -1000 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(static_cast<long int>(-1));
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      if(bashColor)
   		cout << bashPassColor;
      cout << "---Test Passed---";
      if(bashColor)
      	cout << bashNormalColor;
   }
   else
   {
      if(bashColor)
   		cout << bashFailColor;
      cout << "---Test Failed---";
      if(bashColor)
      	cout << bashNormalColor;
      status = false;
   }
   cout << endl << endl;
   if(printSeperator)
   {
      cout << seperator << endl;
   }

///-------------------------------------------------------------------------///
///Seconds - after midnight
   ctime.set(1337L,0,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, -1000 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(static_cast<long int>(-1));
   cout << "Changed Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, 1000 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(static_cast<long int>(1));
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      if(bashColor)
   		cout << bashPassColor;
      cout << "---Test Passed---";
      if(bashColor)
      	cout << bashNormalColor;
   }
   else
   {
      if(bashColor)
   		cout << bashFailColor;
      cout << "---Test Failed---";
      if(bashColor)
      	cout << bashNormalColor;
      status = false;
   }
   cout << endl << endl;
   if(printSeperator)
   {
      cout << seperator << endl;
   }
   
///-------------------------------------------------------------------------///
///Seconds - noon +
   ctime.set(1337L,SEC_BEFORE_MIDNIGHT/2,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, 1000 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(static_cast<long int>(1));
   cout << "Changed Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, -1000 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(static_cast<long int>(-1));
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      if(bashColor)
   		cout << bashPassColor;
      cout << "---Test Passed---";
      if(bashColor)
      	cout << bashNormalColor;
   }
   else
   {
      if(bashColor)
   		cout << bashFailColor;
      cout << "---Test Failed---";
      if(bashColor)
      	cout << bashNormalColor;
      status = false;
   }
   cout << endl << endl;
   if(printSeperator)
   {
      cout << seperator << endl;
   }
   
///-------------------------------------------------------------------------///
///Seconds - noon -
   ctime.set(1337L,SEC_BEFORE_MIDNIGHT/2,0);
   oldct = ctime;
   
   cout << "Start Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, -1000 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(static_cast<long int>(-1));
   cout << "Changed Time: " << ctime << endl;
   if(!silent)
   {
      cout << "Adding: 0 days, 1000 msod, 0 fsod" << endl;
   }
   ctime = ctime.addSeconds(static_cast<long int>(1));
   cout << "End Time: " << ctime << endl;
   if( ctime == oldct )
   {
      if(bashColor)
   		cout << bashPassColor;
      cout << "---Test Passed---";
      if(bashColor)
      	cout << bashNormalColor;
   }
   else
   {
      if(bashColor)
   		cout << bashFailColor;
      cout << "---Test Failed---";
      if(bashColor)
      	cout << bashNormalColor;
      status = false;
   }
   cout << endl << endl;
   if(printSeperator)
   {
      cout << seperator << endl;
   }
   
///-------------------------------------------------------------------------///
   
   return status;
}
