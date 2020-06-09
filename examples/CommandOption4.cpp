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
// CommandOption4
// CommandOption4 --scream
// CommandOption4 --scream --hush
// CommandOption4 -x
// CommandOption4 -x -y -z
// CommandOption4 --hush -xyz

/// Example of using CommandOptionNOf in an application.
class CommandOption4Example : public BasicFramework
{
public:
      /// Initialize command-line arguments
   CommandOption4Example(const string& applName);
      /// Process command-line arguments
   bool initialize(int argc, char *argv[], bool pretty = true) throw() override;
      /// Do the processing.
   void process() override;
      /// Clean up.
   void shutDown() override;
      /// generic option x
   CommandOptionNoArg xOpt;
      /// generic option y
   CommandOptionNoArg yOpt;
      /// generic option z
   CommandOptionNoArg zOpt;
      /// Make sure that x y and z are specified together.
   CommandOptionAllOf xyzOpts;

      /// Option for demonstrating CommandOptionOneOf
   CommandOptionNoArg screamOpt;
      /// Option for demonstrating CommandOptionOneOf
   CommandOptionNoArg hushOpt;
      /// Make sure only one of hushOpt or screamOpt are used
   CommandOptionMutex hushOrScreamOpt;
      /// Make sure that if xyz options are used, hush is specified
   CommandOptionDependent hushAndXYZOpt;
};


CommandOption4Example ::
CommandOption4Example(const string& applName)
      : BasicFramework(applName, "Example application for CommandOption"),
        xOpt('x', "", "you say you want an x"),
        yOpt('y', "", "you say you want an y"),
        zOpt('z', "", "you say you want an z"),
        screamOpt(0, "scream", "print a message very loudly"),
        hushOpt(0, "hush", "don't print a message very loudly"),
        hushAndXYZOpt(&hushOpt, &xyzOpts)
{
      // Define the options that must be used together.
   xyzOpts.addOption(&xOpt);
   xyzOpts.addOption(&yOpt);
   xyzOpts.addOption(&zOpt);
      // Only one of these options may be specified.
   hushOrScreamOpt.addOption(&screamOpt);
   hushOrScreamOpt.addOption(&hushOpt);
}


bool CommandOption4Example ::
initialize(int argc, char *argv[], bool pretty) throw()
{
   if (!BasicFramework::initialize(argc, argv, pretty))
      return false;
   if (screamOpt)
   {
      cout << "HELLO WORLD x" << screamOpt.getCount() << endl;
   }
   if (hushOpt)
   {
      cout << "ok i'll be quiet x" << hushOpt.getCount() << endl;
   }
      // whichOne returns the option that was used
   CommandOption *which = hushOrScreamOpt.whichOne();
   if (which != nullptr)
   {
      cout << "You used " << which->getFullOptionString() << endl;
   }
      // do some processing of x y and z options.
      // ...
   return true;
}


void CommandOption4Example ::
process()
{
   cout << "Nothing to do" << endl;
}


void CommandOption4Example ::
shutDown()
{
   cout << "Shutting down" << endl;
}


int main(int argc, char *argv[])
{
   try
   {
      CommandOption4Example app(argv[0]);
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
