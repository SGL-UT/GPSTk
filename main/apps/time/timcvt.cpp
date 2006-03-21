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






#include "BasicFramework.hpp"
#include "DayTime.hpp"
#include "CommandOptionWithTimeArg.hpp"

using namespace std;
using namespace gpstk;

class TimCvt : public BasicFramework
{
public:
   TimCvt(char* arg0);

protected:
   virtual void process();

private:
   CommandOptionWithTimeArg CalendarTime;
   CommandOptionWithTimeArg RinexTime;
   CommandOptionWithTimeArg RinexFileTime;
   CommandOptionWithTimeArg DOYTime;
   CommandOptionWithTimeArg MJDTime;
   CommandOptionWithTimeArg ShortWeekSOWTime;
   CommandOptionWithTimeArg ShortWeekZCountsTime;
   CommandOptionWithTimeArg FullWeekSOWTime;
   CommandOptionWithTimeArg FullWeekZCountsTime;
   CommandOptionWithTimeArg UnixTime;
   CommandOptionWithTimeArg ZCountsTime;
   CommandOptionWithAnyArg formatOption;
   CommandOptionWithNumberArg addOption;
   CommandOptionWithNumberArg subOption;
   CommandOptionMutex mutexOption;

   string stringToParse;
   string timeSpec;
};

TimCvt::TimCvt(char* arg0)
      : BasicFramework(arg0, "Converts from a given input time specification"
                       " to other time formats.  Include the quotation marks."
                       "  All year values are four digit years."),
        CalendarTime('c', "calendar", "%m %d %Y",
                     "\"Month(numeric) DayOfMonth Year\""),
        RinexTime('r',"rinex","%m %d %Y %H:%M:%S",
                  "\"Month(numeric) DayOfMonth Year Hour:Minute:Second\""),
        RinexFileTime('R', "rinex-file", "%y %m %d %H %M %S",
                      "\"Year(2-digit) Month(numeric) DayOfMonth Hour Minute Second\""),
        DOYTime('y',"doy","%Y %j %s",
                "\"Year DayOfYear SecondsOfDay\""),
        MJDTime('m',"mjd","%Q",
                "\"ModifiedJulianDate\""),
        ShortWeekSOWTime('o',"shortweekandsow","%G %g %Y",
                         "\"10bitGPSweek SecondsOfWeek Year\""),
        ShortWeekZCountsTime('z',"shortweekandzcounts","%G %Z %Y",
                             "\"10bitGPSweek ZCounts Year\""),
        FullWeekSOWTime('f',"fullweekandsow","%F %g",
                        "\"FullGPSweek SecondsOfWeek\""),
        FullWeekZCountsTime('w',"fullweekandzcounts","%F %Z",
                            "\"FullGPSweek ZCounts\""),
        UnixTime('u',"unixtime","%U %u",
                 "\"UnixSeconds UnixMicroseconds\""),
        ZCountsTime('Z',"fullZcounts","%C",
                    "\"fullZcounts\""),
        formatOption('F', "format", "Time format to use on output"),
        addOption('a', "add-offset", "add NUM seconds to specified time"),
        subOption('s', "sub-offset", "subtract NUM seconds from specified time")
{
   CalendarTime.setMaxCount(1);
   RinexTime.setMaxCount(1);
   RinexFileTime.setMaxCount(1);
   DOYTime.setMaxCount(1);
   MJDTime.setMaxCount(1);
   ShortWeekSOWTime.setMaxCount(1);
   ShortWeekZCountsTime.setMaxCount(1);
   FullWeekSOWTime.setMaxCount(1);
   FullWeekZCountsTime.setMaxCount(1);
   UnixTime.setMaxCount(1);
   ZCountsTime.setMaxCount(1);
   formatOption.setMaxCount(1);

   mutexOption.addOption(&CalendarTime);
   mutexOption.addOption(&RinexTime);
   mutexOption.addOption(&RinexFileTime);
   mutexOption.addOption(&DOYTime);
   mutexOption.addOption(&MJDTime);
   mutexOption.addOption(&ShortWeekSOWTime);
   mutexOption.addOption(&ShortWeekZCountsTime);
   mutexOption.addOption(&FullWeekSOWTime);
   mutexOption.addOption(&FullWeekZCountsTime);
   mutexOption.addOption(&UnixTime);
   mutexOption.addOption(&ZCountsTime);
}

void TimCvt::process()
{
   DayTime dt;
      //dt.setToString(stringToParse, timeSpec);
   CommandOptionWithTimeArg* whichOpt =
      (CommandOptionWithTimeArg*)mutexOption.whichOne();

   if (whichOpt)
      dt = whichOpt->getTime().front();

   int i;
   int addOptions = addOption.getCount();
   int subOptions = subOption.getCount();
   for (i = 0; i < addOptions; i++)
      dt += StringUtils::asDouble(addOption.getValue()[i]);
   for (i = 0; i < subOptions; i++)
      dt -= StringUtils::asDouble(subOption.getValue()[i]);

   if (formatOption.getCount())
   {
      cout << dt.printf(formatOption.getValue()[0]) << endl;
   }
   else
   {
      using StringUtils::leftJustify;
      string eight(8, ' '); // eight spaces
      
      cout << endl
           << eight << leftJustify("Month/Day/Year", 32) 
           << dt.printf("%m/%d/%Y") << endl

           << eight << leftJustify("Hour:Min:Sec", 32)
           << dt.printf("%02H:%02M:%02S") << endl

           << eight << leftJustify("Modified Julian Date", 32)
           << setprecision(15) << dt.MJDdate() << endl

           << eight << leftJustify("GPSweek DayOfWeek SecOfWeek", 32)
           << dt.printf("%G %w %g") << endl

           << eight << leftJustify("FullGPSweek Zcount", 32)
           << dt.printf("%F %z") << endl

           << eight << leftJustify("Year DayOfYear SecondOfDay", 32)
           << dt.printf("%Y %j %s") << endl

           << eight << leftJustify("Unix_sec Unix_usec", 32)
           << dt.printf("%U %u") << endl

           << eight << leftJustify("FullZcount", 32)
           << dt.printf("%c") << endl

           << endl << endl;
   }

   return;
}

int main(int argc, char* argv[])
{
   try
   {
      TimCvt tc(argv[0]);
      if (!tc.initialize(argc, argv))
         return 0;
      if (!tc.run())
         return 1;
      
      return 0;
   }
   catch(Exception& e)
   {
      cout << e << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "unknown error" << endl;
   }
   return 0;
}
