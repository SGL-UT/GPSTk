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

/**
 * @file CommandOptionParser.hpp
 * Parse command line options
 */

#ifndef COMMANDOPTIONPARSER_HPP
#define COMMANDOPTIONPARSER_HPP

#include "CommandOption.hpp"

#include <cstring>
#include <vector>
#include <map>
#include <ostream>

namespace gpstk
{
      /// @ingroup CommandLine
      //@{

      /** 
       * This class parses the command line options and modifies the
       * corresponding CommandOptions.  By default, any CommandOptions you 
       * create will be put on a static vector<CommandOption> which is used
       * by CommandOptionParser.  You can make your own as well but that
       * isn't necessary.  You can also use addOption() to add individual
       * CommandOptions to the parser, but again this isn't necessary as the
       * default list is usually sufficient.
       *
       * Call parseOptions() to process the command line, then 
       * call hasErrors() to see if there
       * were any problems parsing the string. Errors can occur when
       * a required option isn't found on the command line, when an option
       * requiring an argument doesn't have one, or when an argument appers
       * more than its maxCount number of times among other errors.
       * If so, use dumpErrors() to
       * display the errors to an output stream, then use
       * displayUsage() to display a well formatted list of the correct
       * command line options.  Of
       * course, you can just as well ignore any command line
       * errors. After hitting an error (which most often happens when
       * it hits an argument that has no CommandOption), you can use
       * CommandOptionRest to get the unprocessed command line
       * options.
       *
       * @sa the getopttest.cpp file in the test code for some examples.
       */
   class CommandOptionParser
   {
   public:
         /// Typedef for a map between the command line option (-f) and the
         /// associated CommandOption.
      typedef std::map<std::string, gpstk::CommandOption*> CommandOptionMap;
      
         /** Constructor given a text description of the program.
          * @warning The CommandOptions in optList must exist for the entire
          *          lifetime of this CommandOptionParser.
          * @param description a short description of this program
          * @param optList a CommandOptionVec with the list of
          *   CommandOptions for this parser.
          */
      CommandOptionParser(const std::string& description,
                          const CommandOptionVec& optList = 
                          defaultCommandOptionList);

         /** Adds the CommandOption to the list for parsing.
          * @warning The CommandOption must exist for the entire
          *          lifetime of this CommandOptionParser.
          */
      CommandOptionParser& addOption(gpstk::CommandOption& co);
      
         /// Parses the command line.
      void parseOptions(int argc, char* argv[]);
      
         /// Returns true if any processing errors occurred.
      bool hasErrors() { return !errorStrings.empty(); }
         /// Writes the errors to \c out.
      std::ostream& dumpErrors(std::ostream& out);
         /// Returns true if any help was requested.
      bool helpRequested() { return !helpOptions.empty(); }
         /** Print the requested help information.
          * @param[in] out The stream to which the help text will be printed.
          * @param[in] pretty If true, use "pretty print" as
          *   appropriate (dependent on child class implementation as
          *   to how it's used).
          * @param[in] firstOnly If true, only print the help for the
          *   first processed command-line option requesting help.  If
          *   false, each specified help-like command-line option will
          *   print its help text. */
      std::ostream& printHelp(std::ostream& out, bool doPretty = true,
                              bool firstOnly = true);

         /** Writes the arguments nicely to the output.
          * @param out ostream on which to write
          * @param doPretty if true (the default), 'pretty print' descriptions
          */
      std::ostream& displayUsage(std::ostream& out, bool doPretty=true);

   private:
         /// changes the size of the option array for getopt_long.
      void resizeOptionArray(struct option* &oldArray, unsigned long& oldSize);
      
         /// The vector of CommandOptions for the parser
      CommandOptionVec optionVec;
         /// The vector of error strings for displaying to the user.
      std::vector<std::string> errorStrings;
         /// The vector of unprocessed command line arguments.
         //std::vector<std::string> remainingArguments;

         /// whether or not this command line has any rrequired options
      bool hasRequiredArguments;
         /// whether or not this command line has optional options
      bool hasOptionalArguments;

         /// the description of this program
      std::string text;

         /// the name of this program
      std::string progName;

         /** After calling parseOptions, this will contain all of the
          * help-like options that were specified on the command
          * line. */
      std::vector<CommandOptionHelp*> helpOptions;
   };
      //@}
}

#endif
