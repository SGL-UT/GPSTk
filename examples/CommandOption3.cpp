#include <iostream>
#include <fstream>
#include <BasicFramework.hpp>
#include <StringUtils.hpp>
#include <CommandOptionWithCommonTimeArg.hpp>
#include <TimeString.hpp>

using namespace std;
using namespace gpstk;

// Given that Doxygen removes Doxygen comments when rendering
// examples, please do not consider the absence of comments in the
// HTML rendering to be representative.  Refer to the file in the
// depot instead.

// Interesting examples:
// CommandOption3 -f "1920 12345" -f "1921 0" --scream
// CommandOption3 -f "1920 12345" -Z 1105530967 --scream
// CommandOption3 -Z 1105530967 -f "1920 12345" --scream

/// Example of using CommandOptionNOf in an application.
class CommandOption3Example : public BasicFramework
{
public:
      /// Initialize command-line arguments
   CommandOption3Example(const string& applName);
      /// Process command-line arguments
   bool initialize(int argc, char *argv[], bool pretty = true) throw() override;
      /// Do the processing.
   void process() override;
      /// Clean up.
   void shutDown() override;
      /// Express time as GPS epoch-week-second
   CommandOptionWithCommonTimeArg gpsEWSOpt;
      /// Express time as GPS fullweek-second
   CommandOptionWithCommonTimeArg gpsWSOpt;
      /// Express time as GPS fullweek-zcount
   CommandOptionWithCommonTimeArg gpsWZOpt;
      /// Express time as GPS 29-bit full zcount
   CommandOptionWithCommonTimeArg gpsZ29Opt;
      /// Express time as GPS 32-bit full zcount
   CommandOptionWithCommonTimeArg gpsZ32Opt;
      /// Make sure that two of the time options have been specified.
   CommandOptionNOf allTimesOption;

      /// Option for demonstrating CommandOptionOneOf
   CommandOptionNoArg screamOpt;
      /// Option for demonstrating CommandOptionOneOf
   CommandOptionNoArg hushOpt;
      /// Make sure only one of hushOpt or screamOpt are used
   CommandOptionOneOf hushScreamOpt;
};


CommandOption3Example ::
CommandOption3Example(const string& applName)
      : BasicFramework(applName, "Example application for CommandOption"),
        gpsEWSOpt('o', "ews", "%E %G %g", 
                     "\"GPSEpoch 10bitGPSweek SecondOfWeek\""),
        gpsWSOpt('f', "ws", "%F %g", "\"FullGPSWeek SecondOfWeek\""),
        gpsWZOpt('w', "wz", "%F %Z", "\"FullGPSWeek Zcount\""),
        gpsZ29Opt(0, "z29", "%E %c", "\"29bitZcount\""),
        gpsZ32Opt('Z', "z32", "%C", "\"32bitZcount\""),
        screamOpt(0, "scream", "print a message very loudly"),
        hushOpt(0, "hush", "don't print a message very loudly"),
        allTimesOption(2) // two and only two of the options must be used
{
      // Define the options that will be used.  As noted above, 2 of
      // these options must be used on the command-line.
   allTimesOption.addOption(&gpsEWSOpt);
   allTimesOption.addOption(&gpsWSOpt);
   allTimesOption.addOption(&gpsWZOpt);
   allTimesOption.addOption(&gpsZ29Opt);
   allTimesOption.addOption(&gpsZ32Opt);
      // At least one of these options must be specified, but there is
      // no restriction on whether more than one is specified
      // (setMaxCount has no effect on CommandOptionOneOf).
   hushScreamOpt.addOption(&screamOpt);
   hushScreamOpt.addOption(&hushOpt);
}


bool CommandOption3Example ::
initialize(int argc, char *argv[], bool pretty) throw()
{
   if (!BasicFramework::initialize(argc, argv, pretty))
      return false;
      // which() constructs a new vector and returns it, so we call
      // which() once and store the data for local use.
   CommandOptionVec timeOpts = allTimesOption.which();
      // timeOpts now contains a vector of all the command-line, so we
      // can iterate through it to figure out what options were used.
   for (unsigned i = 0; i < timeOpts.size(); i++)
   {
         // In this example, the command-line options can be used
         // twice, so we iterate through the values stored in each
         // option.
         // If you want to restrict the command-line so that only a
         // certain number of a given option used in the
         // CommandOptionNOf validation, e.g. only specify GPS
         // week-second once in this example, then use the
         // setMaxCount() method on the individual options.
      for (unsigned j = 0; j < timeOpts[i]->getCount(); j++)
      {
         CommandOptionWithCommonTimeArg* opt =
            (CommandOptionWithCommonTimeArg*)timeOpts[i];
            // Because the values are stored with each command-line
            // option, we use getOrder to determine the order in which
            // the user specified the options on the command-line.
            // This isn't always necessary but the information is here
            // if needed.
         unsigned order = timeOpts[i]->getOrder(j);
         cout << "Option #" << order << " " << opt->getValue()[j]
              << " = "
              << printTime(opt->getTime()[j], "%04Y/%02m/%02d %02H:%02M:%02S")
              << endl;
      }
   }
   if (screamOpt)
   {
      cout << "HELLO WORLD x" << screamOpt.getCount() << endl;
   }
   if (hushOpt)
   {
      cout << "ok i'll be quiet x" << hushOpt.getCount() << endl;
   }
      // whichOne returns the option that was used, which is a little
      // strange given multiple can be used (CommandOptionMutex is
      // better for a "one and only one" relation).
   CommandOption *which = hushScreamOpt.whichOne();
   if (which != nullptr)
   {
      cout << "You used " << which->getFullOptionString()
           << " (and possibly others)" << endl;
   }
   return true;
}


void CommandOption3Example ::
process()
{
   cout << "Nothing to do" << endl;
}


void CommandOption3Example ::
shutDown()
{
   cout << "Shutting down" << endl;
}


int main(int argc, char *argv[])
{
   try
   {
      CommandOption3Example app(argv[0]);
      if (app.initialize(argc, argv))
      {
         app.run();
      }
      return app.exitCode;
   }
   catch (gpstk::Exception& e)
   {
      cerr << e << endl;
   }
   catch (std::exception& e)
   {
      cerr << e.what() << endl;
   }
   catch (...)
   {
      cerr << "Caught unknown exception" << endl;
   }
   return gpstk::BasicFramework::EXCEPTION_ERROR;
}
