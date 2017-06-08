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
//  Copyright 2017, The University of Texas at Austin
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


#include <BasicFramework.hpp>
#include <CommonTime.hpp>
#include <CommandOptionWithCommonTimeArg.hpp>

using namespace std;
using namespace gpstk;

class TimeDiff : public BasicFramework
{
public:
   TimeDiff(char* arg0);

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
   CommandOptionNOf allTimesOption;

   string stringToParse;
   string timeSpec;
};

TimeDiff::TimeDiff(char* arg0)
      : BasicFramework(arg0, "Subtract the second specified time from the"
                       " first and print the result in seconds."),
        ANSITimeOption('A', "ansi", "%K", "\"ANSI-Second\""),
        CivilTimeOption('c', "civil", "%m %d %Y %H:%M:%f",
                        "\"Month(numeric) DayOfMonth Year"
                        " Hour:Minute:Second\""),
        RinexFileTimeOption('R', "rinex-file", "%y %m %d %H %M %S",
                            "\"Year(2-digit) Month(numeric) DayOfMonth Hour"
                            " Minute Second\""),
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
        allTimesOption(2)
{
   allTimesOption.addOption(&ANSITimeOption);
   allTimesOption.addOption(&CivilTimeOption);
   allTimesOption.addOption(&RinexFileTimeOption);
   allTimesOption.addOption(&GPSEWSOption);
   allTimesOption.addOption(&GPSWSOption);
   allTimesOption.addOption(&GPSWZOption);
   allTimesOption.addOption(&GPSZ29Option);
   allTimesOption.addOption(&GPSZ32Option);
   allTimesOption.addOption(&JDOption);
   allTimesOption.addOption(&MJDOption);
   allTimesOption.addOption(&UnixTimeOption);
   allTimesOption.addOption(&YDSTimeOption);
}

void TimeDiff::process()
{
   CommonTime dt;
      //dt.setToString(stringToParse, timeSpec);
   std::vector<CommandOption*> timeOpts = allTimesOption.which();
   std::map<unsigned, CommonTime> orderedTimes;
   std::map<unsigned, CommonTime>::iterator oti1, oti2;
   std::map<unsigned, std::string> orderedStrs;
   std::map<unsigned, std::string>::iterator osi1, osi2;

      // store the time in order of appearance on the command line
   for (unsigned i = 0; i < timeOpts.size(); i++)
   {
      for (unsigned j = 0; j < timeOpts[i]->getCount(); j++)
      {
         CommandOptionWithCommonTimeArg* opt =
            (CommandOptionWithCommonTimeArg*)timeOpts[i];
         unsigned order = timeOpts[i]->getOrder(j);
         orderedTimes[order] = opt->getTime()[j];
         orderedStrs[order] = opt->getValue()[j];
         orderedTimes[order].setTimeSystem(gpstk::TimeSystem::Any);
      }
   }
   oti1 = orderedTimes.begin();
   osi1 = orderedStrs.begin();
   oti2 = oti1;
   osi2 = osi1;
   oti2++;
   osi2++;
   if (verboseLevel)
      cout << osi1->second << " - " << osi2->second << " = ";
   cout << fixed << (oti1->second - oti2->second) << endl;

   return;
}

int main(int argc, char* argv[])
{
   try
   {
      TimeDiff app(argv[0]);
      if (!app.initialize(argc, argv))
         return app.exitCode;
      app.run();
      return app.exitCode;
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
   return gpstk::BasicFramework::EXCEPTION_ERROR;
}
