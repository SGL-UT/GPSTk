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

#include "CommandOption.hpp"
#include "TestUtil.hpp"
#include <iostream>

using namespace std;
using namespace gpstk;

/*************************************************************************
 * This class tests the creation of the various types of CommandOptions.
 */
class CommandOption_T
{
public:

   int testCommandOption();
   int testRequiredOption();
   int testCommandOptionNoArg();
   int testCommandOptionWithArg();
   int testCommandOptionWithAnyArg();
   int testCommandOptionWithStringArg();
   int testCommandOptionWithNumberArg();
   int testCommandOptionWithDecimalArg();
   int testCommandOptionRest();
   int testCommandOptionNOf();
   int testCommandOptionOneOf();
   int testCommandOptionAllOf();
   int testCommandOptionMutex();
   int testCommandOptionDependent();
   int testCommandOptionGroupOr();
   int testCommandOptionGroupAnd();

   CommandOption_T() : verboseLevel(0) { init(); }
   ~CommandOption_T() { }

   void init() { }

   int  verboseLevel;

}; // class CommandOption_T


/*************************************************************************
 */
int CommandOption_T::testCommandOption()
{
   TestUtil  tester( "CommandOption", "Initialization", __FILE__, __LINE__ );

   CommandOptionVec  testCmdOptVec;
   int  expectedCount = 0;

   try   // No arg, trailing, no flags
   {
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::trailingType, 0, "", "", false, testCmdOptVec);
      tester.assert( true, "CommandOption was created successfully.", __LINE__ );
      ++expectedCount;
      tester.assert( (cmdOpt.getArgString().compare("ARG") == 0), "CommandOption getArgString() should return ARG.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOption count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getValue().size() == 0), "CommandOption value size should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getOrder() == 0), "CommandOption order should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() == 0), "CommandOption checkArguments() should return nothing.", __LINE__ );
      tester.assert( (testCmdOptVec.size() == expectedCount), "CommandOption was not added to the supplied vector.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOption() threw an exception but should not have.", __LINE__ );
   }

   try   // No arg, standard, no flags
   {
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType, 0, "", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( false, "CommandOption creation should have failed due to missing short and long options.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOption() threw an exception as expected.", __LINE__ );
   }

   try   // No arg, standard, short flag (valid)
   {
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType, 'f', "", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( true, "CommandOption was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getOptionString().compare("-f") == 0),
                     "CommandOption getOptionString() returned '" + cmdOpt.getFullOptionString() + "', expected '-f'",
                     __LINE__ );
      tester.assert( (cmdOpt.getFullOptionString().compare("  -f") == 0),
                     "CommandOption getFullOptionString() returned '" + cmdOpt.getFullOptionString() + "', expected '  -f'",
                     __LINE__ );
      tester.assert( (testCmdOptVec.size() == expectedCount), "CommandOption was not added to the supplied vector.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOption() threw an exception as expected.", __LINE__ );
   }

   try   // No arg, standard, short flag (bogus)
   {
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType, ' ', "", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( false, "CommandOption creation should have failed due to invalid short option.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOption() threw an exception as expected.", __LINE__ );
   }

   try   // No arg, standard, long flag (valid)
   {
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType, 0, "foo", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( true, "CommandOption was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getOptionString().compare("--foo") == 0),
                     "CommandOption getOptionString() returned '" + cmdOpt.getFullOptionString() + "', expected '--foo'",
                     __LINE__ );
      tester.assert( (cmdOpt.getFullOptionString().compare("      --foo") == 0),
                     "CommandOption getFullOptionString() returned '" + cmdOpt.getFullOptionString() + "', expected '      --foo'",
                     __LINE__ );
      tester.assert( (testCmdOptVec.size() == expectedCount), "CommandOption was not added to the supplied vector.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOption() threw an exception as expected.", __LINE__ );
   }

   try   // No arg, standard, long flag (bogus)
   {
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType, 0, "foo bar", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( false, "CommandOption creation should have failed due to invalid long option.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOption() threw an exception as expected.", __LINE__ );
   }

   try   // No arg, standard, both flags (valid)
   {
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType, 'f', "foo", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( true, "CommandOption was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getOptionString().compare("-f | --foo") == 0),
                     "CommandOption getOptionString() returned '" + cmdOpt.getOptionString() + "', expected '-f | --foo'",
                     __LINE__ );
      tester.assert( (cmdOpt.getFullOptionString().compare("  -f, --foo") == 0),
                     "CommandOption getFullOptionString() returned '" + cmdOpt.getFullOptionString() + "', expected '  -f, --foo'",
                     __LINE__ );
      tester.assert( (testCmdOptVec.size() == expectedCount), "CommandOption was not added to the supplied vector.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOption() threw an exception but should not have.", __LINE__ );
   }

   try   // No arg, meta, no flags
   {
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::metaType, 0, "", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( true, "CommandOption was created successfully.", __LINE__ );
      tester.assert( (testCmdOptVec.size() == expectedCount), "CommandOption was not added to the supplied vector.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOption() threw an exception but should not have.", __LINE__ );
   }

   try   // Arg, trailing, no flags
   {
      CommandOption  cmdOpt(CommandOption::hasArgument, CommandOption::trailingType, 0, "", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( true, "CommandOption was created successfully.", __LINE__ );
      tester.assert( (testCmdOptVec.size() == expectedCount), "CommandOption was not added to the supplied vector.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOption() threw an exception but should not have.", __LINE__ );
   }

   try   // Arg, standard, no flags
   {
      CommandOption  cmdOpt(CommandOption::hasArgument, CommandOption::stdType, 0, "", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( false, "CommandOption creation should have failed due to missing short and long options.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOption() threw an exception as expected.", __LINE__ );
   }

   try   // Arg, standard, short flag (valid)
   {
      CommandOption  cmdOpt(CommandOption::hasArgument, CommandOption::stdType, 'f', "", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( true, "CommandOption was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getOptionString().compare("-f") == 0),
                     "CommandOption getOptionString() returned '" + cmdOpt.getFullOptionString() + "', expected '-f'",
                     __LINE__ );
      tester.assert( (cmdOpt.getFullOptionString().compare("  -f  ARG") == 0),
                     "CommandOption getFullOptionString() returned '" + cmdOpt.getFullOptionString() + "', expected '  -f  ARG'",
                     __LINE__ );
      tester.assert( (testCmdOptVec.size() == expectedCount), "CommandOption was not added to the supplied vector.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOption() threw an exception as expected.", __LINE__ );
   }

   try   // Arg, standard, short flag (bogus)
   {
      CommandOption  cmdOpt(CommandOption::hasArgument, CommandOption::stdType, ' ', "", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( false, "CommandOption creation should have failed due to invalid short option.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOption() threw an exception as expected.", __LINE__ );
   }

   try   // Arg, standard, long flag (valid)
   {
      CommandOption  cmdOpt(CommandOption::hasArgument, CommandOption::stdType, 0, "foo", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( true, "CommandOption was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getOptionString().compare("--foo") == 0),
                     "CommandOption getOptionString() returned '" + cmdOpt.getFullOptionString() + "', expected '--foo'",
                     __LINE__ );
      tester.assert( (cmdOpt.getFullOptionString().compare("      --foo=ARG") == 0),
                     "CommandOption getFullOptionString() returned '" + cmdOpt.getFullOptionString() + "', expected '      --foo=ARG'",
                     __LINE__ );
      tester.assert( (testCmdOptVec.size() == expectedCount), "CommandOption was not added to the supplied vector.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOption() threw an exception as expected.", __LINE__ );
   }

   try   // Arg, standard, long flag (bogus)
   {
      CommandOption  cmdOpt(CommandOption::hasArgument, CommandOption::stdType, 0, "foo bar", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( false, "CommandOption creation should have failed due to invalid long option.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOption() threw an exception as expected.", __LINE__ );
   }

   try   // Arg, standard, both flags (valid)
   {
      CommandOption  cmdOpt(CommandOption::hasArgument, CommandOption::stdType, 'f', "foo", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( true, "CommandOption was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getOptionString().compare("-f | --foo") == 0),
                     "CommandOption getOptionString() returned '" + cmdOpt.getOptionString() + "', expected '-f | --foo'",
                     __LINE__ );
      tester.assert( (cmdOpt.getFullOptionString().compare("  -f, --foo=ARG") == 0),
                     "CommandOption getFullOptionString() returned '" + cmdOpt.getFullOptionString() + "', expected '  -f, --foo=ARG'",
                     __LINE__ );
      tester.assert( (testCmdOptVec.size() == expectedCount), "CommandOption was not added to the supplied vector.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOption() threw an exception but should not have.", __LINE__ );
   }

   // @todo - Test getDescription()

   try   // Arg, meta, no flags
   {
      CommandOption  cmdOpt(CommandOption::hasArgument, CommandOption::metaType, 0, "", "", false, testCmdOptVec);
      ++expectedCount;
      tester.assert( true, "CommandOption was created successfully.", __LINE__ );
      tester.assert( (testCmdOptVec.size() == expectedCount), "CommandOption was not added to the supplied vector.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOption() threw an exception but should not have.", __LINE__ );
   }

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testRequiredOption()
{
   TestUtil  tester( "RequiredOption", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try   // No arg, trailing
   {
      RequiredOption  cmdOpt(CommandOption::noArgument, CommandOption::trailingType, 0, "", "");
      tester.assert( true, "RequiredOption was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "RequiredOption count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getValue().size() == 0), "RequiredOption value size should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getOrder() == 0), "RequiredOption order should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() != 0), "RequiredOption checkArguments() should have returned an error", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "RequiredOption was not added to the default list.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "RequiredOption() threw an exception but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testCommandOptionNoArg()
{
   TestUtil  tester( "CommandOptionNoArg", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionNoArg  cmdOpt(0, "", "", false);
      tester.assert( false, "CommandOptionNoArg creation should have failed due to missing short and long options.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOptionNoArg() threw an exception as expected.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionNoArg  cmdOpt('f', "foo", "Foo", false);
      tester.assert( true, "CommandOptionNoArg was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOptionNoArg count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getValue().size() == 0), "CommandOptionNoArg value size should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getOrder() == 0), "CommandOptionNoArg order should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() == 0), "CommandOptionNoArg checkArguments() should return nothing.", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "CommandOptionNoArg was not added to the default list.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionNoArg() threw an exception but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testCommandOptionWithArg()
{
   TestUtil  tester( "CommandOptionWithArg", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionWithArg  cmdOpt(CommandOption::stdType, 0, "", "", false);
      tester.assert( false, "CommandOptionWithArg creation should have failed due to missing short and long options.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOptionWithArg() threw an exception as expected.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionWithArg  cmdOpt(CommandOption::stdType, 'f', "foo", "Foo", false);
      tester.assert( true, "CommandOptionWithArg was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOptionWithArg count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getValue().size() == 0), "CommandOptionWithArg value size should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getOrder() == 0), "CommandOptionWithArg order should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() == 0), "CommandOptionWithArg checkArguments() should return nothing.", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "CommandOptionWithArg was not added to the default list.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionWithArg() threw an exception but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testCommandOptionWithAnyArg()
{
   TestUtil  tester( "CommandOptionWithAnyArg", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionWithAnyArg  cmdOpt(0, "", "", false);
      tester.assert( false, "CommandOptionWithAnyArg creation should have failed due to missing short and long options.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOptionWithAnyArg() threw an exception as expected.", __LINE__ );
   }
   
   defaultCommandOptionList.clear();

   try
   {
      CommandOptionWithAnyArg  cmdOpt('f', "foo", "Foo", false);
      tester.assert( true, "CommandOptionWithAnyArg was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOptionWithAnyArg count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getValue().size() == 0), "CommandOptionWithAnyArg value size should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getOrder() == 0), "CommandOptionWithAnyArg order should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() == 0), "CommandOptionWithAnyArg checkArguments() should return nothing.", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "CommandOptionWithAnyArg was not added to the default list.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionWithAnyArg() threw an exception but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testCommandOptionWithStringArg()
{
   TestUtil  tester( "CommandOptionWithStringArg", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionWithStringArg  cmdOpt(0, "", "", false);
      tester.assert( false, "CommandOptionWithStringArg creation should have failed due to missing short and long options.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOptionWithStringArg() threw an exception as expected.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionWithStringArg  cmdOpt('f', "foo", "Foo", false);
      tester.assert( true, "CommandOptionWithStringArg was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOptionWithStringArg count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getValue().size() == 0), "CommandOptionWithStringArg value size should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getOrder() == 0), "CommandOptionWithStringArg order should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() == 0), "CommandOptionWithStringArg checkArguments() should return nothing.", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "CommandOptionWithStringArg was not added to the default list.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionWithStringArg() threw an exception but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testCommandOptionWithNumberArg()
{
   TestUtil  tester( "CommandOptionWithNumberArg", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionWithNumberArg  cmdOpt(0, "", "", false);
      tester.assert( false, "CommandOptionWithNumberArg creation should have failed due to missing short and long options.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOptionWithNumberArg() threw an exception as expected.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionWithNumberArg  cmdOpt('f', "foo", "Foo", false);
      tester.assert( true, "CommandOptionWithNumberArg was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOptionWithNumberArg count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getValue().size() == 0), "CommandOptionWithNumberArg value size should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getOrder() == 0), "CommandOptionWithNumberArg order should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() == 0), "CommandOptionWithNumberArg checkArguments() should return nothing.", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "CommandOptionWithNumberArg was not added to the default list.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionWithNumberArg() threw an exception but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testCommandOptionWithDecimalArg()
{
   TestUtil  tester( "CommandOptionWithDecimalArg", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionWithDecimalArg  cmdOpt(0, "", "", false);
      tester.assert( false, "CommandOptionWithDecimalArg creation should have failed due to missing short and long options.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOptionWithDecimalArg() threw an exception as expected.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionWithDecimalArg  cmdOpt('f', "foo", "Foo", false);
      tester.assert( true, "CommandOptionWithDecimalArg was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOptionWithDecimalArg count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getValue().size() == 0), "CommandOptionWithDecimalArg value size should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getOrder() == 0), "CommandOptionWithDecimalArg order should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() == 0), "CommandOptionWithDecimalArg checkArguments() should return nothing.", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "CommandOptionWithDecimalArg was not added to the default list.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionWithDecimalArg() threw an exception but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testCommandOptionRest()
{
   TestUtil  tester( "CommandOptionRest", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionRest  cmdOpt("", false);
      tester.assert( true, "CommandOptionRest was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOptionRest count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getValue().size() == 0), "CommandOptionRest value size should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getOrder() == 0), "CommandOptionRest order should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() == 0), "CommandOptionRest checkArguments() should return nothing.", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "CommandOptionRest was not added to the default list.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionRest() threw an exception but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testCommandOptionNOf()
{
   TestUtil  tester( "CommandOptionNOf", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionNOf  cmdOpt(1);
      tester.assert( true, "CommandOptionNOf was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOptionNOf count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() != 0), "CommandOptionNOf checkArguments() should have returned an error", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "CommandOptionNOf was not added to the default list.", __LINE__ );

      try
      {
         cmdOpt.addOption(NULL);
         tester.assert( false, "CommandOptionNOf()::addOption() succeeded but should have failed due to an valid option address.", __LINE__ );
      }
      catch ( ... )
      {
         tester.assert( true, "CommandOptionNOf::addOption() threw an exception as expected.", __LINE__ );
      }

      try
      {
         CommandOptionWithAnyArg  cowaa('f', "foo", "Foo", false);
         cmdOpt.addOption(&cowaa);
         tester.assert( true, "CommandOptionNOf()::addOption() succeeded.", __LINE__ );
      }
      catch ( ... )
      {
         tester.assert( false, "CommandOptionNOf::addOption() threw an exception but should not have.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionNOf() threw an exception but should not have.", __LINE__ );
   }
   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testCommandOptionOneOf()
{
   TestUtil  tester( "CommandOptionOneOf", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionOneOf  cmdOpt;
      tester.assert( true, "CommandOptionOneOf was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOptionOneOf count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() != 0), "CommandOptionOneOf checkArguments() should have reported an error", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "CommandOptionOneOf was not added to the default list.", __LINE__ );

      try
      {
         cmdOpt.addOption(NULL);
         tester.assert( false, "CommandOptionOneOf()::addOption() succeeded but should have failed due to an valid option address.", __LINE__ );
      }
      catch ( ... )
      {
         tester.assert( true, "CommandOptionOneOf::addOption() threw an exception as expected.", __LINE__ );
      }

      try
      {
         CommandOptionWithAnyArg  cowaa('f', "foo", "Foo", false);
         cmdOpt.addOption(&cowaa);
         tester.assert( true, "CommandOptionOneOf()::addOption() succeeded.", __LINE__ );
      }
      catch ( ... )
      {
         tester.assert( false, "CommandOptionOneOf::addOption() threw an exception but should not have.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionOneOf() threw an exception but should not have.", __LINE__ );
   }
   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testCommandOptionAllOf()
{
   TestUtil  tester( "CommandOptionAllOf", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionAllOf  cmdOpt;
      tester.assert( true, "CommandOptionAllOf was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOptionAllOf count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() == 0), "CommandOptionAllOf checkArguments() should return nothing.", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "CommandOptionAllOf was not added to the default list.", __LINE__ );

      try
      {
         cmdOpt.addOption(NULL);
         tester.assert( false, "CommandOptionAllOf()::addOption() succeeded but should have failed due to an valid option address.", __LINE__ );
      }
      catch ( ... )
      {
         tester.assert( true, "CommandOptionAllOf::addOption() threw an exception as expected.", __LINE__ );
      }

      try
      {
         CommandOptionWithAnyArg  cowaa('f', "foo", "Foo", false);
         cmdOpt.addOption(&cowaa);
         tester.assert( true, "CommandOptionAllOf()::addOption() succeeded.", __LINE__ );
      }
      catch ( ... )
      {
         tester.assert( false, "CommandOptionAllOf::addOption() threw an exception but should not have.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionAllOf() threw an exception but should not have.", __LINE__ );
   }
   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testCommandOptionMutex()
{
   TestUtil  tester( "CommandOptionMutex", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionMutex  cmdOpt(false);
      tester.assert( true, "CommandOptionMutex was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOptionMutex count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() == 0), "CommandOptionMutex checkArguments() should return nothing.", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "CommandOptionMutex was not added to the default list.", __LINE__ );

      try
      {
         cmdOpt.addOption(NULL);
         tester.assert( false, "CommandOptionMutex()::addOption() succeeded but should have failed due to an valid option address.", __LINE__ );
      }
      catch ( ... )
      {
         tester.assert( true, "CommandOptionMutex::addOption() threw an exception as expected.", __LINE__ );
      }

      try
      {
         CommandOptionWithAnyArg  cowaa('f', "foo", "Foo", false);
         cmdOpt.addOption(&cowaa);
         tester.assert( true, "CommandOptionMutex()::addOption() succeeded.", __LINE__ );
      }
      catch ( ... )
      {
         tester.assert( false, "CommandOptionMutex::addOption() threw an exception but should not have.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionMutex() threw an exception but should not have.", __LINE__ );
   }
   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testCommandOptionDependent()
{
   TestUtil  tester( "CommandOptionDependent", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionDependent  cmdOpt(NULL, NULL);
      tester.assert( false, "CommandOptionDependent creation should have failed due to NULL addresses.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOptionDependent() threw an exception as expected.", __LINE__ );
   }
   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testCommandOptionGroupOr()
{
   TestUtil  tester( "CommandOptionGroupOr", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionGroupOr  cmdOpt;
      tester.assert( true, "CommandOptionGroupOr was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOptionGroupOr count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() == 0), "CommandOptionGroupOr checkArguments() should return nothing.", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "CommandOptionGroupOr was not added to the default list.", __LINE__ );

      try
      {
         cmdOpt.addOption(NULL);
         tester.assert( false, "CommandOptionGroupOr()::addOption() succeeded but should have failed due to an valid option address.", __LINE__ );
      }
      catch ( ... )
      {
         tester.assert( true, "CommandOptionGroupOr::addOption() threw an exception as expected.", __LINE__ );
      }

      try
      {
         CommandOptionWithAnyArg  cowaa('f', "foo", "Foo", false);
         cmdOpt.addOption(&cowaa);
         tester.assert( true, "CommandOptionGroupOr()::addOption() succeeded.", __LINE__ );
      }
      catch ( ... )
      {
         tester.assert( false, "CommandOptionGroupOr::addOption() threw an exception but should not have.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionGroupOr() threw an exception but should not have.", __LINE__ );
   }
   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOption_T::testCommandOptionGroupAnd()
{
   TestUtil  tester( "CommandOptionGroupAnd", "Initialization", __FILE__, __LINE__ );

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionGroupAnd  cmdOpt;
      tester.assert( true, "CommandOptionGroupAnd was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOptionGroupAnd count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() == 0), "CommandOptionGroupAnd checkArguments() should return nothing.", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "CommandOptionGroupAnd was not added to the default list.", __LINE__ );

      try
      {
         cmdOpt.addOption(NULL);
         tester.assert( false, "CommandOptionGroupAnd()::addOption() succeeded but should have failed due to an valid option address.", __LINE__ );
      }
      catch ( ... )
      {
         tester.assert( true, "CommandOptionGroupAnd::addOption() threw an exception as expected.", __LINE__ );
      }

      try
      {
         CommandOptionWithAnyArg  cowaa('f', "foo", "Foo", false);
         cmdOpt.addOption(&cowaa);
         tester.assert( true, "CommandOptionGroupAnd()::addOption() succeeded.", __LINE__ );
      }
      catch ( ... )
      {
         tester.assert( false, "CommandOptionGroupAnd::addOption() threw an exception but should not have.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionGroupAnd() threw an exception but should not have.", __LINE__ );
   }
   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 * Run the program.
 *
 * @return Total error count for all tests
 */
int main(int argc, char *argv[])
{
   int  errorTotal = 0;

   CommandOption_T  testClass;

   errorTotal += testClass.testCommandOption();
   errorTotal += testClass.testRequiredOption();
   errorTotal += testClass.testCommandOptionNoArg();
   errorTotal += testClass.testCommandOptionWithArg();
   errorTotal += testClass.testCommandOptionWithAnyArg();
   errorTotal += testClass.testCommandOptionWithStringArg();
   errorTotal += testClass.testCommandOptionWithNumberArg();
   errorTotal += testClass.testCommandOptionWithDecimalArg();
   errorTotal += testClass.testCommandOptionRest();
   errorTotal += testClass.testCommandOptionNOf();
   errorTotal += testClass.testCommandOptionOneOf();
   errorTotal += testClass.testCommandOptionAllOf();
   errorTotal += testClass.testCommandOptionMutex();
   errorTotal += testClass.testCommandOptionDependent();
   errorTotal += testClass.testCommandOptionGroupOr();
   errorTotal += testClass.testCommandOptionGroupAnd();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal << std::endl;

   return( errorTotal );
   
}  // main()
