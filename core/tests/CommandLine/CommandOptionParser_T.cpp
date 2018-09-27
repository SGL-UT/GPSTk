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

#include "CommandOptionParser.hpp"
#include "CommandOptionWithCommonTimeArg.hpp"
#include "YDSTime.hpp"
#include "TestUtil.hpp"
#include <iostream>

using namespace std;
using namespace gpstk;

// Macro to assert that there are no errors and add diagnostics to
// test output if there are
#define COPANOERR(COP)                                                  \
   {                                                                    \
      std::ostringstream oss;                                           \
      oss << "CommandOptionParser has errors:" << endl;                 \
      COP.dumpErrors(oss);                                              \
      testFramework.assert(!COP.hasErrors(), oss.str(), __LINE__);      \
   }

class CommandOptionParser_T
{
public:
   CommandOptionParser_T() { }
   ~CommandOptionParser_T() { }

   int testInitialization();
   int testAddOption();
   int testParseOptions();
   int testOptionPresence();
   int testNOfWhich();
      //int testDisplayUsage();

};


/*************************************************************************
 */
int CommandOptionParser_T::testInitialization()
{
   TUDEF("CommandOptionParser", "Initialization");

   try
   {
      CommandOptionParser  cop("");
      TUASSERT(!cop.hasErrors());
      TUPASS("CommandOptionParser was created successfully.");
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception but should not have.");
   }

   try
   {
      CommandOptionParser  cop("Program description");
      TUASSERT(!cop.hasErrors());
      TUPASS("CommandOptionParser was created successfully.");
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception but should not have.");
   }

   try
   {
      CommandOptionVec  testCmdOptVec;
      CommandOptionParser  cop("Program description", testCmdOptVec);
      TUASSERT(!cop.hasErrors());
      TUPASS("CommandOptionParser was created successfully.");
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception but should not have.");
   }

   try
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument,
                             CommandOption::stdType, 'f', "foo", "Foo", false,
                             testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument,
                             CommandOption::stdType, 'b', "bar", "Boo", false,
                             testCmdOptVec);
      CommandOptionParser  cop("Program description", testCmdOptVec);
      TUASSERT(!cop.hasErrors());
      TUPASS("CommandOptions were added successfully.");
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception but should not have.");
   }

   try  // Disallow multiple CommandOption's with identical short options
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::stdType,
                             'f', "foo", "Foo", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::stdType,
                             'f', "far", "Far", false, testCmdOptVec);
      CommandOptionParser  cop("Program description", testCmdOptVec);
      TUFAIL("CommandOptionParser should have disallowed conflicting short"
             " options.");
   }
   catch ( ... )
   {
      TUPASS("CommandOptionParser correctly threw an exception to disallow"
             " conflicting short options.");
   }

   try  // Disallow multiple CommandOption's with identical long options
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::stdType,
                             'f', "foo", "Foo1", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::stdType,
                             'F', "foo", "Foo2", false, testCmdOptVec);
      CommandOptionParser  cop("Program description", testCmdOptVec);
      TUFAIL("CommandOptionParser should have disallowed conflicting long"
             " options.");
   }
   catch ( ... )
   {
      TUPASS("CommandOptionParser correctly threw an exception to disallow"
             " conflicting long options.");
   }

   try  // Disallow multiple CommandOptionRest instances
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument,
                             CommandOption::trailingType, 0, "", "Foo1", false,
                             testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument,
                             CommandOption::trailingType, 0, "", "Foo2", false,
                             testCmdOptVec);
      CommandOptionParser  cop("Program description", testCmdOptVec);
      TUFAIL("CommandOptionParser should have disallowed multiple"
             " CommandOptionRest instances.");
   }
   catch ( ... )
   {
      TUPASS("CommandOptionParser correctly threw an exception to multiple"
             " CommandOptionRest instances.");
   }

   TURETURN();
}


/*************************************************************************
 */
int CommandOptionParser_T::testAddOption()
{
   TUDEF("CommandOptionParser", "AddOption");

   try
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::stdType,
                             'f', "foo", "Foo", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::stdType,
                             'b', "bar", "Boo", false, testCmdOptVec);
      CommandOptionParser  cop("Program description");
      cop.addOption(cmdOpt1);
      cop.addOption(cmdOpt2);
      TUPASS("CommandOptions were added successfully.");
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception but should not have.");
   }

   try  // Disallow multiple CommandOption's with identical short options
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::stdType,
                             'f', "foo", "Foo", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::stdType,
                             'f', "far", "Far", false, testCmdOptVec);
      CommandOptionParser  cop("Program description");
      cop.addOption(cmdOpt1);
      cop.addOption(cmdOpt2);
      TUFAIL("CommandOptionParser should have disallowed conflicting short"
             " options.");
   }
   catch ( ... )
   {
      TUPASS("CommandOptionParser correctly threw an exception to disallow"
             " conflicting short options.");
   }

   try  // Disallow multiple CommandOption's with identical long options
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::stdType,
                             'f', "foo", "Foo1", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::stdType,
                             'F', "foo", "Foo2", false, testCmdOptVec);
      CommandOptionParser  cop("Program description");
      cop.addOption(cmdOpt1);
      cop.addOption(cmdOpt2);
      TUFAIL("CommandOptionParser should have disallowed conflicting long"
             " options.");
   }
   catch ( ... )
   {
      TUPASS("CommandOptionParser correctly threw an exception to disallow"
             " conflicting long options.");
   }

   try  // Disallow multiple CommandOptionRest instances
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument,
                             CommandOption::trailingType, 0, "", "Foo1", false,
                             testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument,
                             CommandOption::trailingType, 0, "", "Foo2", false,
                             testCmdOptVec);
      CommandOptionParser  cop("Program description");
      cop.addOption(cmdOpt1);
      cop.addOption(cmdOpt2);
      TUFAIL("CommandOptionParser should have disallowed multiple"
             " CommandOptionRest instances.");
   }
   catch ( ... )
   {
      TUPASS("CommandOptionParser correctly threw an exception to multiple"
             " CommandOptionRest instances.");
   }

   TURETURN();
}


/*************************************************************************
 */
int CommandOptionParser_T::testParseOptions()
{
   TUDEF("CommandOptionParser", "ParseOptions");

   try  // Parse with no CommandOptions
   {
      CommandOptionParser  cop("Description");
      int  argc = 1;
      char*  argv[] = { const_cast<char*>("program") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   try  // Parse with a single CommandOption with no value
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType,
                            'f', "foo", "Foo", false, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 2;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f")};
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors())
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         TUASSERTE(unsigned long,1,cmdOpt.getCount());
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         TUASSERTE(unsigned long,1,cmdOpt.getOrder());
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an unexpected exception.");
   }

   try  // Parse with a single CommandOption with a value
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::hasArgument, CommandOption::stdType,
                            'f', "foo", "Foo", false, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 3;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f"),
                        const_cast<char*>("value") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         TUASSERTE(unsigned long,1,cmdOpt.getCount());
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         TUASSERTE(unsigned long,1,cmdOpt.getOrder());
         std::vector<std::string>  values = cmdOpt.getValue();
         TUASSERTE(unsigned long,1,values.size());
         if (values.size() == 1)
         {
            TUASSERTE(unsigned long,0,values[0].compare("value"));
         }
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an unexpected exception.");
   }

   try  // Parse with an unexpected standard CommandOption
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType,
                            'f', "foo", "Foo", false, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 2;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-g")};
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   try  // Parse with an unexpected trailing CommandOption
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType,
                            'f', "foo", "Foo", false, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 3;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   try  // Parse with a missing required CommandOption
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType,
                            'f', "foo", "Foo", true, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 2;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   try  // Parse with a violated CommandOption max count
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType,
                            'f', "foo", "Foo", false, testCmdOptVec);
      cmdOpt.setMaxCount(1);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 3;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f"),
                        const_cast<char*>("-f") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   try  // Parse with multiple CommandOptions
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::stdType,
                             'f', "foo", "Foo", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::stdType,
                             'g', "goo", "Goo", false, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 3;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-g"),
                        const_cast<char*>("-f") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         std::ostringstream  count1Oss;
         count1Oss << cmdOpt1.getCount();         
         TUASSERTE(unsigned long,1,cmdOpt1.getCount());
         std::ostringstream  count2Oss;
         count2Oss << cmdOpt1.getCount();         
         TUASSERTE(unsigned long,1,cmdOpt2.getCount());
         std::ostringstream  order1Oss;
         order1Oss << cmdOpt2.getOrder();
         TUASSERTE(unsigned long,2,cmdOpt1.getOrder());
         std::ostringstream  order2Oss;
         order2Oss << cmdOpt2.getOrder();
         TUASSERTE(unsigned long,1,cmdOpt2.getOrder());
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   try  // Parse a CommandOption with no short option
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType,
                            0, "foo", "Foo", false, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 2;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("--foo") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         TUASSERTE(unsigned long,1,cmdOpt.getCount());
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         TUASSERTE(unsigned long,1,cmdOpt.getOrder());
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   try  // Parse a CommandOption with no long option
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType,
                            'f', "", "Foo", false, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 2;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f")};
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         TUASSERTE(unsigned long,1,cmdOpt.getCount());
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         TUASSERTE(unsigned long,1,cmdOpt.getOrder());
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionNoArg
   {
      CommandOptionNoArg  cmdOpt('f', "foo", "Foo", false);
      CommandOptionParser  cop("Description");
      int  argc = 3;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f"),
                        const_cast<char*>("--foo") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         TUASSERTE(unsigned long,2,cmdOpt.getCount());
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         TUASSERTE(unsigned long,2,cmdOpt.getOrder());
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithArg
   {
      CommandOptionWithArg  cmdOpt(CommandOption::stdType, 'f', "foo", "Foo",
                                   false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("value2") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         TUASSERTE(unsigned long,2,cmdOpt.getCount());
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         TUASSERTE(unsigned long,2,cmdOpt.getOrder());
         std::vector<std::string>  values = cmdOpt.getValue();
         TUASSERTE(unsigned long,2,values.size());
         if (values.size() == 2)
         {
            TUASSERTE(std::string,"value1",values[0]);
            TUASSERTE(std::string,"value2",values[1]);
         }
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithAnyArg
   {
      CommandOptionWithAnyArg  cmdOpt('f', "foo", "Foo", false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("value2") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         TUASSERTE(unsigned long,2,cmdOpt.getCount());
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         TUASSERTE(unsigned long,2,cmdOpt.getOrder());
         std::vector<std::string>  values = cmdOpt.getValue();
         TUASSERTE(unsigned long,2,values.size());
         if (values.size() == 2)
         {
            TUASSERTE(std::string,"value1",values[0]);
            TUASSERTE(std::string,"value2",values[1]);
         }
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithStringArg (invalid)
   {
      CommandOptionWithStringArg  cmdOpt('f', "foo", "Foo", false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-f"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("value2") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered expected errors while"
          << " parsing: ";
         cop.dumpErrors(oss);
         TUPASS(oss.str());
      }
      else
      {
         TUFAIL("CommandOptionParser parsed without errors but should have"
                " rejected the argument value due to its format");
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithStringArg (valid)
   {
      CommandOptionWithStringArg  cmdOpt('f', "foo", "Foo", false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f"),
                        const_cast<char*>("valueOne"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("valueTwo") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         TUASSERTE(unsigned long,2,cmdOpt.getCount());
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         TUASSERTE(unsigned long,2,cmdOpt.getOrder());
         std::vector<std::string>  values = cmdOpt.getValue();
         TUASSERTE(unsigned long,2,values.size());
         if (values.size() == 2)
         {
            TUASSERTE(std::string,"valueOne",values[0]);
            TUASSERTE(std::string,"valueTwo",values[1]);
         }
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithNumberArg (invalid)
   {
      CommandOptionWithNumberArg  cmdOpt('f', "foo", "Foo", false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-f"),
                        const_cast<char*>("value"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("12.45") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered expected errors while"
          << " parsing: ";
         cop.dumpErrors(oss);
         TUPASS(oss.str());
      }
      else
      {
         TUFAIL("CommandOptionParser parsed without errors but should have"
                " rejected the argument value due to its format");
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithNumberArg (valid)
   {
      CommandOptionWithNumberArg  cmdOpt('f', "foo", "Foo", false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-f"),
                        const_cast<char*>("0"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("12345") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         TUASSERTE(unsigned long,2,cmdOpt.getCount());
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         TUASSERTE(unsigned long,2,cmdOpt.getOrder());
         std::vector<std::string>  values = cmdOpt.getValue();
         TUASSERTE(unsigned long,2,values.size());
         if (values.size() == 2)
         {
            TUASSERTE(std::string,"0",values[0]);
            TUASSERTE(std::string,"12345",values[1]);
         }
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithDecimalArg (invalid)
   {
      CommandOptionWithDecimalArg  cmdOpt('f', "foo", "Foo", false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-f"),
                        const_cast<char*>("value"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("1.2e34") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered expected errors while"
          << " parsing: ";
         cop.dumpErrors(oss);
         TUPASS(oss.str());
      }
      else
      {
         TUFAIL("CommandOptionParser parsed without errors but should have"
                " rejected the argument value due to its format");
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithDecimalArg (valid)
   {
      CommandOptionWithDecimalArg  cmdOpt('f', "foo", "Foo", false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-f"),
                        const_cast<char*>("0"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("123.45") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         TUASSERTE(unsigned long,2,cmdOpt.getCount());
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         TUASSERTE(unsigned long,2,cmdOpt.getOrder());
         std::vector<std::string>  values = cmdOpt.getValue();
         TUASSERTE(unsigned long,2,values.size());
         if (values.size() == 2)
         {
            TUASSERTE(std::string,"0",values[0]);
            TUASSERTE(std::string,"123.45",values[1]);
         }
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithCommonTimeArg (invalid)
   {
      CommandOptionWithCommonTimeArg  cmdOpt('t', "time", "%Y %j %s", "Time",
                                             false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-t"),
                        const_cast<char*>("value"),
                        const_cast<char*>("--time"),
                        const_cast<char*>("1234") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered expected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUPASS(oss.str());
      }
      else
      {
         TUFAIL("CommandOptionParser parsed without errors but should have"
                " rejected the argument value due to its format.");
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithCommonTimeArg (valid YDS)
   {
      CommandOptionWithCommonTimeArg  cmdOpt('t', "time", "%Y %j %s", "Time",
                                             false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-t"),
                        const_cast<char*>("2015 123 45678.0"),
                        const_cast<char*>("--time"),
                        const_cast<char*>("2015 234 56789.0") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         TUASSERTE(unsigned long,2,cmdOpt.getCount());
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         TUASSERTE(unsigned long,2,cmdOpt.getOrder());
         std::vector<std::string>  values = cmdOpt.getValue();
         TUASSERTE(unsigned long,2,values.size());
         if (values.size() == 2)
         {
            TUASSERTE(std::string,"2015 123 45678.0",values[0]);
            TUASSERTE(std::string,"2015 234 56789.0",values[1]);
         }
         std::vector<gpstk::CommonTime>  times = cmdOpt.getTime();
         TUASSERTE(unsigned long,2,times.size());
         if (times.size() == 2)
         {
            gpstk::CommonTime  t1 =
               gpstk::YDSTime(2015, 123, 45678.0).convertToCommonTime();
            gpstk::CommonTime  t2 =
               gpstk::YDSTime(2015, 234, 56789.0).convertToCommonTime();
            TUASSERTE(gpstk::CommonTime,t1,times[0]);
            TUASSERTE(gpstk::CommonTime,t2,times[1]);
         }
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an unexpected exception.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a missing required CommandOptionRest
   {
      CommandOptionRest  cmdOpt("Description", true);
      CommandOptionParser  cop("Description");
      int  argc = 1;
      char*  argv[] = { const_cast<char*>("program") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionRest
   {
      CommandOptionRest  cmdOpt("Description", false);
      CommandOptionParser  cop("Description");
      int  argc = 3;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("trailing1"),
                        const_cast<char*>("trailing2") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         TUASSERTE(unsigned long,2,cmdOpt.getCount());
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         TUASSERTE(unsigned long,2,cmdOpt.getOrder());
         std::vector<std::string>  values = cmdOpt.getValue();
         TUASSERTE(unsigned long,2,values.size());
         if (values.size() == 2)
         {
            TUASSERTE(std::string,"trailing1",values[0]);
            TUASSERTE(std::string,"trailing2",values[1]);
         }
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   TURETURN();
}


/*************************************************************************
 */
int CommandOptionParser_T::testOptionPresence()
{
   TUDEF("CommandOptionParser", "OptionPresence");

   try  // Parse with a satisfied CommandOptionMutex
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionMutex  com;
      com.addOption(&cmdOptF);
      com.addOption(&cmdOptB);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         TUASSERTE(CommandOption*,&cmdOptF,com.whichOne());
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a violated CommandOptionMutex
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionMutex  com;
      com.addOption(&cmdOptF);
      com.addOption(&cmdOptB);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("-B"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a violated CommandOptionMutex
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionMutex  com;
      com.addOption(&cmdOptF);
      com.addOption(&cmdOptRest);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("-B"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a satisfied CommandOptionDependent
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionDependent  cod(&cmdOptF, &cmdOptB);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("-B"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a satisfied CommandOptionDependent
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionDependent  cod(&cmdOptB, &cmdOptRest);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("-B"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a violated CommandOptionDependent
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionDependent  cod(&cmdOptB, &cmdOptF);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a violated CommandOptionDependent
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionDependent  cod(&cmdOptB, &cmdOptRest);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a satisfied CommandOptionNOf
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionNOf  conof(0);
      conof.addOption(&cmdOptB);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
            // @todo - Check which() result
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a satisfied CommandOptionNOf
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionNOf  conof(1);
      conof.addOption(&cmdOptF);
      conof.addOption(&cmdOptB);
      CommandOptionParser  cop("Description");
      int  argc = 4;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
            // @todo - Check which() result
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a satisfied CommandOptionNOf
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionNOf  conof(2);
      conof.addOption(&cmdOptF);
      conof.addOption(&cmdOptB);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("-B"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
            // @todo - Check which() result
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();
   
   try  // Parse with a satisfied CommandOptionNOf
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionNOf  conof(2);
      conof.addOption(&cmdOptF);
      conof.addOption(&cmdOptB);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
            // @todo - Check which() result
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();
   
   try  // Parse with a violated CommandOptionNOf
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionNOf  conof(0);
      conof.addOption(&cmdOptB);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("-B"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a violated CommandOptionNOf
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionNOf  conof(1);
      conof.addOption(&cmdOptF);
      conof.addOption(&cmdOptB);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("-B"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a violated CommandOptionNOf
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionNOf  conof(3);
      conof.addOption(&cmdOptF);
      conof.addOption(&cmdOptB);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("-B"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a satisfied CommandOptionOneOf
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionOneOf  cooo;
      cooo.addOption(&cmdOptB);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("-B"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         TUASSERTE(CommandOption*,&cmdOptB,cooo.whichOne());
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a satisfied CommandOptionOneOf
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionOneOf  cooo;
      cooo.addOption(&cmdOptF);
      cooo.addOption(&cmdOptRest);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         TUASSERTE(CommandOption*,&cmdOptF,cooo.whichOne());
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a violated CommandOptionOneOf
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionOneOf  cooo;
      cooo.addOption(&cmdOptB);
      cooo.addOption(&cmdOptRest);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("value2") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a satisfied CommandOptionAllOf
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionAllOf  coao;
      coao.addOption(&cmdOptB);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("-B"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a satisfied CommandOptionAllOf
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionAllOf  coao;
      coao.addOption(&cmdOptF);
      coao.addOption(&cmdOptB);
      coao.addOption(&cmdOptRest);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("-B"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a violated CommandOptionAllOf
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionAllOf  coao;
      coao.addOption(&cmdOptF);
      coao.addOption(&cmdOptB);
      coao.addOption(&cmdOptRest);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

#ifdef BROKENTEST

      /** JMK 2015/11/17 : These tests are based on an incorrect
       * understanding of the purpose behind GroupOr and GroupAnd.
       * Neither of these meta-options imply any sort of requirements,
       * they are just ways to do boolean logic on groups of options,
       * to be subsequently used in options that *do* have
       * requirements. */

   try  // Parse with a satisfied CommandOptionGroupOr
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionGroupOr  cogo;
      cogo.addOption(&cmdOptF);
      cogo.addOption(&cmdOptB);
      CommandOptionDependent(&cogo, &cmdOptRest);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         TUASSERTE(CommandOption*,&cmdOptF,cogo.whichOne());
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a unsatisfied CommandOptionGroupOr
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionWithAnyArg  cmdOptJ('J', "jig", "Jig", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionGroupOr  cogo;
      cogo.addOption(&cmdOptF);
      cogo.addOption(&cmdOptB);
      CommandOptionDependent(&cogo, &cmdOptRest);
      CommandOptionParser  cop("Description");
      int  argc = 4;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-J"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a satisfied CommandOptionGroupAnd
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionGroupAnd  coga;
      coga.addOption(&cmdOptF);
      coga.addOption(&cmdOptB);
      CommandOptionDependent(&coga, &cmdOptRest);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("-B"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while"
             << " parsing: ";
         cop.dumpErrors(oss);
         TUFAIL(oss.str());
      }
      else
      {
         TUPASS("CommandOptionParser parsed without errors.");
         TUASSERTE(CommandOption*,&cmdOptF,coga.whichOne());
      }
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();

   try  // Parse with a unsatisfied CommandOptionGroupAnd
   {
      CommandOptionWithAnyArg  cmdOptF('F', "foo", "Foo", false);
      CommandOptionWithAnyArg  cmdOptB('B', "bar", "Bar", false);
      CommandOptionRest  cmdOptRest("Rest", false);
      CommandOptionGroupAnd  coga;
      coga.addOption(&cmdOptF);
      coga.addOption(&cmdOptB);
      CommandOptionDependent(&coga, &cmdOptRest);
      CommandOptionParser  cop("Description");
      int  argc = 6;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-F"),
                        const_cast<char*>("value1"),
                        const_cast<char*>("--foo"),
                        const_cast<char*>("value2"),
                        const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      TUPASS("CommandOptionParser parsed the options without throwing an"
             " exception.");
      TUASSERT(cop.hasErrors());
   }
   catch ( ... )
   {
      TUFAIL("CommandOptionParser() threw an exception while parsing but"
             " should not have.");
   }

   defaultCommandOptionList.clear();
#endif

   TURETURN();
}


void testNOfWhichRpt(unsigned expWhich, gpstk::TestUtil& testFramework,
                     unsigned argc, char *argv[])
{
   try
   {
      defaultCommandOptionList.clear();
      CommandOptionWithAnyArg cmdOpt1('f', "foo", "Foo", false);
      CommandOptionWithAnyArg cmdOpt2('b', "bar", "Bar", false);
      CommandOptionWithAnyArg cmdOpt3('B', "baz", "Baz", false);
      CommandOptionNOf nof(2);
      CommandOptionParser cop("testNOfWhich");
      nof.addOption(&cmdOpt1);
      nof.addOption(&cmdOpt2);
      nof.addOption(&cmdOpt3);
      TUPASS("Constructed objects");
      cop.parseOptions(argc, argv);
         // based on the construction of nof and argv, only argc==5
         // can be valid
      if (argc == 5)
      {
         COPANOERR(cop);
         std::vector<CommandOption*> witches = nof.which();
         TUASSERTE(unsigned, expWhich, witches.size());
      }
      else
      {
         TUASSERT(cop.hasErrors());
      }
   }
   catch (...)
   {
      TUFAIL("Unexpected exception");
   }
}


int CommandOptionParser_T :: testNOfWhich()
{
   TUDEF("CommandOptionNOf", "which");

      // test a pair of different arguments
   char* argv1[] =
      {                                  // argc (argv index+1)
         const_cast<char*>("program1"),  // 1
         const_cast<char*>("-f"),        // 2
         const_cast<char*>("wub1"),      // 3
         const_cast<char*>("-b"),        // 4
         const_cast<char*>("wub2"),      // 5
         const_cast<char*>("-B"),        // 6
         const_cast<char*>("wub3")       // 7
      };

      // test a pair of identical arguments
   char* argv2[] =
      {                                  // argc (argv index+1)
         const_cast<char*>("program2"),  // 1
         const_cast<char*>("-f"),        // 2
         const_cast<char*>("wub1"),      // 3
         const_cast<char*>("-f"),        // 4
         const_cast<char*>("wub2")       // 5
      };

   for (unsigned argc = 1; argc <= 7; argc++)
      testNOfWhichRpt(2, testFramework, argc, argv1);

   testNOfWhichRpt(1, testFramework, 5, argv2);

   TURETURN();
}


/** Run the program.
 *
 * @return Total error count for all tests
 */
int main(int argc, char *argv[])
{
   int  errorTotal = 0;

   CommandOptionParser_T  testClass;

   errorTotal += testClass.testInitialization();
   errorTotal += testClass.testAddOption();
   errorTotal += testClass.testParseOptions();
   errorTotal += testClass.testOptionPresence();
   errorTotal += testClass.testNOfWhich();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;

   return( errorTotal );
   
}  // main()
