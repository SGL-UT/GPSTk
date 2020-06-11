#include <iostream>
#include <fstream>
#include <BasicFramework.hpp>

using namespace std;
using namespace gpstk;

// Given that Doxygen removes Doxygen comments when rendering
// examples, please do not consider the absence of comments in the
// HTML rendering to be representative.  Refer to the file in the
// depot instead.

// Interesting examples:
// CommandOption1  -f CPackConfig.cmake --scream foo bar baz

/// Class to show how to add a simple command-line argument to an application.
class CommandOption1Example : public BasicFramework
{
public:
      /// Initialize command-line arguments
   CommandOption1Example(const string& applName);
      /// Process command-line arguments
   bool initialize(int argc, char *argv[], bool pretty = true) throw() override;
      /// Do the processing.
   void process() override;
      /// Clean up.
   void shutDown() override;
      /// Command-line option example
   CommandOptionWithAnyArg fileOpt;
      /// Another example
   CommandOptionNoArg screamOpt;
      /// Everything else after the above command-line options
   CommandOptionRest restOpt;
};


CommandOption1Example ::
CommandOption1Example(const string& applName)
      : BasicFramework(applName, "Example application for CommandOption"),
        fileOpt('f', "file", "input file", true),
        screamOpt(0, "scream", "print a message very loudly"),
        restOpt("FILE [...]", true)
{
   fileOpt.setMaxCount(1);
}


bool CommandOption1Example ::
initialize(int argc, char *argv[], bool pretty) throw()
{
   if (!BasicFramework::initialize(argc, argv, pretty))
      return false;
      // fileOpt is required and thus no checking need be done.  We
      // can just get the first argument.  If there was no argument or
      // too many instances of fileOpt specified, the above check will
      // have returned false already.
   ifstream fs(fileOpt.getValue()[0].c_str());
   if (!fs)
   {
         // print an error message for the intended file
      std::perror(fileOpt.getValue()[0].c_str());
         // pretty safe to assume it's that the file doesn't exist
      exitCode = BasicFramework::EXIST_ERROR;
         // We're done.  Stop the program.
      return false;
   }
   if (screamOpt)
      cout << "HELLO WORLD" << endl;
   const vector<string>& restVec = restOpt.getValue();
   cout << "Remaining values:" << endl;
   for (unsigned i = 0; i < restVec.size(); i++)
   {
      cout << "  #" << i << " = " << restVec[i] << endl;
   }
   return true;
}


void CommandOption1Example ::
process()
{
   cout << "Nothing to process" << endl;
}


void CommandOption1Example ::
shutDown()
{
   cout << "Shutting down" << endl;
}


int main(int argc, char *argv[])
{
   try
   {
      CommandOption1Example app(argv[0]);
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
