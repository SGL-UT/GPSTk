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
// CommandOption2 -n -1 -D 1.2 -D 3.4 -t "2020/4/5 1:2:3"

/** Class to show how to use command-line options with format checking
 * in an application. */
class CommandOption2Example : public BasicFramework
{
public:
      /// Initialize command-line arguments
   CommandOption2Example(const string& applName);
      /// Process command-line arguments
   bool initialize(int argc, char *argv[], bool pretty = true) throw() override;
      /// Do the processing.
   void process() override;
      /// Clean up.
   void shutDown() override;
      /// Command-line option example with integer checks
   CommandOptionWithNumberArg numOpt;
      /// Command-line option example with float checks
   CommandOptionWithDecimalArg floatOpt;
      /// Command-line option example for time
   CommandOptionWithCommonTimeArg timeOpt;
      /// storage for integer
   int num;
      /// the decimal choices
   std::vector<double> dub;
      /// requested start time
   gpstk::CommonTime beginTime;
};


CommandOption2Example ::
CommandOption2Example(const string& applName)
      : BasicFramework(applName, "Example application for CommandOption"),
        numOpt('n', "num", "your choice of integer"),
        floatOpt('D', "data", "specify any number of decimal numbers"),
        timeOpt('t', "time", "%Y/%m/%d %H:%M:%S",
                "specify a time (%Y/%m/%d %H:%M:%S)", true),
        num(0)
{
   numOpt.setMaxCount(1);
   timeOpt.setMaxCount(1);
}


bool CommandOption2Example ::
initialize(int argc, char *argv[], bool pretty) throw()
{
   if (!BasicFramework::initialize(argc, argv, pretty))
      return false;
      // numOpt is not required and so check if it was actually specified.
   if (numOpt.getCount() > 0)
   {
         // convert the string value to integer
      num = gpstk::StringUtils::asInt(numOpt.getValue()[0]);
   }
      // floatOpt is also not required
   if (floatOpt.getCount() > 0)
   {
      const std::vector<std::string>& vals(floatOpt.getValue());
         // make our target storage the same size as the source so we
         // don't have to reallocate over and over.
      dub.resize(vals.size(), 0./0.);
      for (unsigned i = 0; i < vals.size(); i++)
      {
         dub[i] = gpstk::StringUtils::asDouble(vals[i]);
      }
   }
      // timeOpt is required, and the getTime method returns a CommonTime object
   beginTime = timeOpt.getTime()[0];
   
   return true;
}


void CommandOption2Example ::
process()
{
   cout << "Your number is " << num << endl;
   cout << "Data:" << endl;
   for (unsigned i = 0; i < dub.size(); i++)
   {
      cout << "  dub[" << i << "] = " << dub[i] << endl;
   }
   cout << "Requested time:"
        << printTime(beginTime, "%Y/%02m/%02d %02H:%02M:%02S")
        << endl;
}


void CommandOption2Example ::
shutDown()
{
   cout << "Shutting down" << endl;
}


int main(int argc, char *argv[])
{
   try
   {
      CommandOption2Example app(argv[0]);
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
