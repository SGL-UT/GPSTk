#pragma ident "$Id$"

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
//  Copyright 2009, The University of Texas at Austin
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

#include "TimeString.hpp"
#include "TimeConstants.hpp"

#include "ANSITime.hpp"
#include "CivilTime.hpp"
#include "GPSWeekSecond.hpp"
#include "GPSWeekZcount.hpp"
#include "JulianDate.hpp"
#include "MJD.hpp"
#include "UnixTime.hpp"
#include "YDSTime.hpp"
#include "SystemTime.hpp"

#include "CommandOptionWithCommonTimeArg.hpp"

using namespace std;
using namespace gpstk;

class TimCvt : public BasicFramework
{
public:
   TimCvt(char* arg0);

protected:
   virtual void process();

private:
   CommandOptionWithCommonTimeArg ANSITimeOption;
   CommandOptionWithCommonTimeArg CivilTimeOption;
   CommandOptionWithCommonTimeArg RinexFileTimeOption;
   CommandOptionWithCommonTimeArg GPSEWSOption;
   CommandOptionWithCommonTimeArg GPSWSOption;
   CommandOptionWithCommonTimeArg GPSWZOption;
   CommandOptionWithCommonTimeArg GPSZ29Option;
   CommandOptionWithCommonTimeArg GPSZ32Option;
   CommandOptionWithCommonTimeArg JDOption;
   CommandOptionWithCommonTimeArg MJDOption;
   CommandOptionWithCommonTimeArg UnixTimeOption;
   CommandOptionWithCommonTimeArg YDSTimeOption;

   CommandOptionWithAnyArg inputFormatOption;
   CommandOptionWithAnyArg inputTimeOption;
   CommandOptionAllOf inputFormatAndTimeOption;

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
        ANSITimeOption('A', "ansi", "%K", "\"ANSI-Second\""),
        CivilTimeOption('c', "civil", "%m %d %Y %H:%M:%f",
                        "\"Month(numeric) DayOfMonth Year Hour:Minute:Second\""),
        RinexFileTimeOption('R', "rinex-file", "%y %m %d %H %M %S",
                            "\"Year(2-digit) Month(numeric) DayOfMonth Hour Minute Second\""),
        GPSEWSOption('o', "ews", "%E %G %g", 
                     "\"GPSEpoch 10bitGPSweek SecondOfWeek\""),
        GPSWSOption('f', "ws", "%F %g", "\"FullGPSWeek SecondOfWeek\""),
        GPSWZOption('w', "wz", "%F %Z", "\"FullGPSWeek Zcount\""),
        GPSZ29Option(0, "z29", "%E %c", "\"29bitZcount\""),
        GPSZ32Option('Z', "z32", "%C", "\"32bitZcount\""),
        JDOption('j', "julian", "%J", "\"JulianDate\""),
        MJDOption('m', "mjd", "%Q", "\"ModifiedJulianDate\""),
        UnixTimeOption('u',"unixtime", "%U %u",
                       "\"UnixSeconds UnixMicroseconds\""),
        YDSTimeOption('y', "doy", "%Y %j %s",
                      "\"Year DayOfYear SecondsOfDay\""),
        inputFormatOption(0, "input-format", "Time format to use on input"),
        inputTimeOption(0, "input-time",
                        "Time to be parsed by \"input-format\" option"),
        formatOption('F', "format", "Time format to use on output"),
        addOption('a', "add-offset", "add NUM seconds to specified time"),
        subOption('s', "sub-offset",
                  "subtract NUM seconds from specified time")
{
   ANSITimeOption.setMaxCount(1);
   CivilTimeOption.setMaxCount(1);
   RinexFileTimeOption.setMaxCount(1);
   GPSEWSOption.setMaxCount(1);
   GPSWSOption.setMaxCount(1);
   GPSWZOption.setMaxCount(1);
   GPSZ29Option.setMaxCount(1);
   GPSZ32Option.setMaxCount(1);
   JDOption.setMaxCount(1);
   MJDOption.setMaxCount(1);
   UnixTimeOption.setMaxCount(1);
   YDSTimeOption.setMaxCount(1);
   formatOption.setMaxCount(1);

   inputFormatOption.setMaxCount(1);
   inputTimeOption.setMaxCount(1);
   inputFormatAndTimeOption.addOption(&inputFormatOption);
   inputFormatAndTimeOption.addOption(&inputTimeOption);

   mutexOption.addOption(&ANSITimeOption);
   mutexOption.addOption(&CivilTimeOption);
   mutexOption.addOption(&RinexFileTimeOption);
   mutexOption.addOption(&GPSEWSOption);
   mutexOption.addOption(&GPSWSOption);
   mutexOption.addOption(&GPSWZOption);
   mutexOption.addOption(&GPSZ29Option);
   mutexOption.addOption(&GPSZ32Option);
   mutexOption.addOption(&JDOption);
   mutexOption.addOption(&MJDOption);
   mutexOption.addOption(&UnixTimeOption);
   mutexOption.addOption(&YDSTimeOption);
   mutexOption.addOption(&inputFormatAndTimeOption);
}

void TimCvt::process()
{
   CommonTime ct;
   CommandOption *whichOpt = mutexOption.whichOne();

   if (whichOpt)
   {
      CommandOptionWithCommonTimeArg *cta = 
         dynamic_cast<CommandOptionWithCommonTimeArg *>(whichOpt);
      if (cta)
      {
         ct = cta->getTime().front();
      }
      else // whichOpt == &inputFormatAndTimeOption
      {
         mixedScanTime( ct, 
                        inputTimeOption.getValue().front(),
                        inputFormatOption.getValue().front() );
      }
   }
   else
   {
      ct = SystemTime(); 
   }

   int i;
   int addOptions = addOption.getCount();
   int subOptions = subOption.getCount();
   for (i = 0; i < addOptions; i++)
      ct += StringUtils::asDouble(addOption.getValue()[i]);
   for (i = 0; i < subOptions; i++)
      ct -= StringUtils::asDouble(subOption.getValue()[i]);

   if (formatOption.getCount())
   {
      cout << printTime(ct, formatOption.getValue()[0]) << endl;
   }
   else
   {
      using StringUtils::leftJustify;
      string eight(8, ' '); // eight spaces
      
      GPSWeekZcount wz(ct);
      CivilTime civ(ct);

      cout << endl
           << eight << leftJustify("Month/Day/Year H:M:S", 32) 
           << CivilTime(ct) << endl

           << eight << leftJustify("Modified Julian Date", 32)
           << setprecision(15) << MJD(ct) << endl

           << eight << leftJustify("GPSweek DayOfWeek SecOfWeek", 32)
           << GPSWeekSecond(ct).printf("%G %w % 13.6g") << endl

           << eight << leftJustify("FullGPSweek Zcount", 32)
           << wz.printf("%F % 6z") << endl

           << eight << leftJustify("Year DayOfYear SecondOfDay", 32)
           << YDSTime(ct).printf("%Y %03j % 12.6s") << endl

           << eight << leftJustify("Unix: Second Microsecond", 32)
           << UnixTime(ct).printf("%U % 6u") << endl

           << eight << leftJustify("Zcount: 29-bit (32-bit)", 32)
           << wz.printf("%c (%C)") << endl

           << endl << endl;
   }

   return;
}

int main(int argc, char* argv[])
{
   TimCvt tc(argv[0]);
   if (!tc.initialize(argc, argv))
      return 0;
   if (!tc.run())
      return 1;
   
   return 0;
}
