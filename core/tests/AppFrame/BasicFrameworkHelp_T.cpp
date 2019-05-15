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
//  Copyright 2018, The University of Texas at Austin
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
#include <cerrno>

/** @file BasicFrameworkHelp_T.cpp Facilitate testing of various
 * functions of the help command-line options. */

/// Specialized help-like command-line option.
class CommandOptionHelpTest : public gpstk::CommandOptionHelp
{
public:
   CommandOptionHelpTest()
         : CommandOptionHelp(noArgument, 'x', "xhelp", "Print x help")
   {}
   virtual ~CommandOptionHelpTest() {}
   virtual void printHelp(std::ostream& out, bool pretty = true)
   {
      out << "Welcome to the help for x" << std::endl;
   }
};

/// Specialized help-like option that takes an argument.
class CommandOptionHelpTestArg : public gpstk::CommandOptionHelp
{
public:
   CommandOptionHelpTestArg()
         : CommandOptionHelp(hasArgument, 'y', "yhelp", "Print y help")
   {}
   virtual ~CommandOptionHelpTestArg() {}
   virtual void printHelp(std::ostream& out, bool pretty = true)
   {
      out << "Welcome to the help for y." << std::endl
          << "Values:" << std::endl;
      for (unsigned i = 0; i < value.size(); i++)
         out << "  " << value[i] << std::endl;
   }
};


class BasicFrameworkHelp_T : public gpstk::BasicFramework
{
public:
   BasicFrameworkHelp_T(const std::string& applName) throw();
   bool initialize(int argc, char *argv[], bool pretty = true) throw();
   CommandOptionHelpTest xOpt;
   CommandOptionHelpTestArg yOpt;
   gpstk::CommandOptionHelpSimple sOpt;
   std::string execName;
};


BasicFrameworkHelp_T ::
BasicFrameworkHelp_T(const std::string& applName)
throw()
      : BasicFramework(applName, "Facilitate testing of help-like options"),
        sOpt('w', "whelp", "It was just a coincidence, I swear.",
             "Odd groups got left, even groups got right. That means 1, 3, 5,\n"
             " 7 left; 2, 4, 6, 8 right. 7 & 8 are whelp groups.\n")
{
   execName = applName;
   std::string::size_type p = std::string::npos;
      // find and remove the path
   p = execName.find_last_of("/\\");
   if (p != std::string::npos)
   {
      execName.erase(0, p+1);
   }
      // remove windows extension if any
   p = execName.find_last_of('.');
   if (p != std::string::npos)
   {
      execName.erase(p);
   }
}


bool BasicFrameworkHelp_T ::
initialize(int argc, char *argv[], bool pretty)
   throw()
{
      // Change behavior slightly based on the application name.  This
      // is neither unusual nor unprecedented.
   if (execName == "BasicFrameworkHelp_T")
   {
         // Do nothing extra, use default behavior of no additional options.
   }
   else if (execName == "BasicFrameworkHelpReq_T")
   {
         // Add a required option to make sure behavior is appropriate
         // in that case
      gpstk::CommandOptionNoArg *reqOpt = new gpstk::CommandOptionNoArg
         ('z', "zreq", "Random required opt", true);
   }
   else
   {
      std::cerr << "Executable name \"" << execName << "\" is not known"
                << std::endl;
   }
   return BasicFramework::initialize(argc, argv, pretty);
}


int main(int argc, char *argv[])
{
   try
   {
      BasicFrameworkHelp_T app(argv[0]);

      if (!app.initialize(argc, argv))
         return app.exitCode;

      app.run();
      return app.exitCode;
   }
   catch (gpstk::Exception &exc)
   {
      std::cerr << exc << std::endl;
   }
   catch (std::exception &exc)
   {
      std::cerr << "Caught std::exception " << exc.what() << std::endl;
   }
   catch (...)
   {
      std::cerr << "Caught unknown exception";
      if (errno)
      {
         std::cerr << ": " << std::strerror(errno);
      }
      std::cerr << std::endl;
   }

   return gpstk::BasicFramework::EXCEPTION_ERROR;
}
