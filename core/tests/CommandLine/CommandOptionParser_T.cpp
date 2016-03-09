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

class CommandOptionParser_T
{
public:
   CommandOptionParser_T() { }
   ~CommandOptionParser_T() { }

   int testInitialization();
   int testAddOption();
   int testParseOptions();
   int testOptionPresence();
   //int testDisplayUsage();

};


/*************************************************************************
 */
int CommandOptionParser_T::testInitialization()
{
   TestUtil  tester( "CommandOptionParser", "Initialization", __FILE__, __LINE__ );

   try
   {
      CommandOptionParser  cop("");
      tester.assert( (cop.hasErrors() == false), "CommandOptionParser has unexpected errors.", __LINE__ );
      tester.assert( true, "CommandOptionParser was created successfully.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception but should not have.", __LINE__ );
   }

   try
   {
      CommandOptionParser  cop("Program description");
      tester.assert( (cop.hasErrors() == false), "CommandOptionParser has unexpected errors.", __LINE__ );
      tester.assert( true, "CommandOptionParser was created successfully.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception but should not have.", __LINE__ );
   }

   try
   {
      CommandOptionVec  testCmdOptVec;
      CommandOptionParser  cop("Program description", testCmdOptVec);
      tester.assert( (cop.hasErrors() == false), "CommandOptionParser has unexpected errors.", __LINE__ );
      tester.assert( true, "CommandOptionParser was created successfully.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception but should not have.", __LINE__ );
   }

   try
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::stdType, 'f', "foo", "Foo", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::stdType, 'b', "bar", "Boo", false, testCmdOptVec);
      CommandOptionParser  cop("Program description", testCmdOptVec);
      tester.assert( (cop.hasErrors() == false), "CommandOptionParser has unexpected errors.", __LINE__ );
      tester.assert( true, "CommandOptions were added successfully.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception but should not have.", __LINE__ );
   }

   try  // Disallow multiple CommandOption's with identical short options
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::stdType, 'f', "foo", "Foo", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::stdType, 'f', "far", "Far", false, testCmdOptVec);
      CommandOptionParser  cop("Program description", testCmdOptVec);
      tester.assert( false, "CommandOptionParser should have disallowed conflicting short options.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOptionParser correctly threw an exception to disallow conflicting short options.", __LINE__ );
   }

   try  // Disallow multiple CommandOption's with identical long options
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::stdType, 'f', "foo", "Foo1", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::stdType, 'F', "foo", "Foo2", false, testCmdOptVec);
      CommandOptionParser  cop("Program description", testCmdOptVec);
      tester.assert( false, "CommandOptionParser should have disallowed conflicting long options.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOptionParser correctly threw an exception to disallow conflicting long options.", __LINE__ );
   }

   try  // Disallow multiple CommandOptionRest instances
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::trailingType, 0, "", "Foo1", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::trailingType, 0, "", "Foo2", false, testCmdOptVec);
      CommandOptionParser  cop("Program description", testCmdOptVec);
      tester.assert( false, "CommandOptionParser should have disallowed multiple CommandOptionRest instances.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOptionParser correctly threw an exception to multiple CommandOptionRest instances.", __LINE__ );
   }

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOptionParser_T::testAddOption()
{
   TestUtil  tester( "CommandOptionParser", "AddOption", __FILE__, __LINE__ );

   try
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::stdType, 'f', "foo", "Foo", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::stdType, 'b', "bar", "Boo", false, testCmdOptVec);
      CommandOptionParser  cop("Program description");
      cop.addOption(cmdOpt1);
      cop.addOption(cmdOpt2);
      tester.assert( true, "CommandOptions were added successfully.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception but should not have.", __LINE__ );
   }

   try  // Disallow multiple CommandOption's with identical short options
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::stdType, 'f', "foo", "Foo", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::stdType, 'f', "far", "Far", false, testCmdOptVec);
      CommandOptionParser  cop("Program description");
      cop.addOption(cmdOpt1);
      cop.addOption(cmdOpt2);
      tester.assert( false, "CommandOptionParser should have disallowed conflicting short options.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOptionParser correctly threw an exception to disallow conflicting short options.", __LINE__ );
   }

   try  // Disallow multiple CommandOption's with identical long options
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::stdType, 'f', "foo", "Foo1", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::stdType, 'F', "foo", "Foo2", false, testCmdOptVec);
      CommandOptionParser  cop("Program description");
      cop.addOption(cmdOpt1);
      cop.addOption(cmdOpt2);
      tester.assert( false, "CommandOptionParser should have disallowed conflicting long options.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOptionParser correctly threw an exception to disallow conflicting long options.", __LINE__ );
   }

   try  // Disallow multiple CommandOptionRest instances
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::trailingType, 0, "", "Foo1", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::trailingType, 0, "", "Foo2", false, testCmdOptVec);
      CommandOptionParser  cop("Program description");
      cop.addOption(cmdOpt1);
      cop.addOption(cmdOpt2);
      tester.assert( false, "CommandOptionParser should have disallowed multiple CommandOptionRest instances.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOptionParser correctly threw an exception to multiple CommandOptionRest instances.", __LINE__ );
   }

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOptionParser_T::testParseOptions()
{
   TestUtil  tester( "CommandOptionParser", "ParseOptions", __FILE__, __LINE__ );

   try  // Parse with no CommandOptions
   {
      CommandOptionParser  cop("Description");
      int  argc = 1;
      char*  argv[] = { const_cast<char*>("program") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   try  // Parse with a single CommandOption with no value
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType, 'f', "foo", "Foo", false, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 2;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         tester.assert( (cmdOpt.getCount() == 1), "Expected 1 instance of CommandOption, not " + countOss.str(), __LINE__ );
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         tester.assert( (cmdOpt.getOrder() == 1), "Expected CommandOption to be option 1, not " + orderOss.str(), __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   try  // Parse with a single CommandOption with a value
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::hasArgument, CommandOption::stdType, 'f', "foo", "Foo", false, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 3;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f"), const_cast<char*>("value") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         tester.assert( (cmdOpt.getCount() == 1), "Expected 1 instance of CommandOption, not " + countOss.str(), __LINE__ );
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         tester.assert( (cmdOpt.getOrder() == 1), "Expected CommandOption to be option 1, not " + orderOss.str(), __LINE__ );
         std::vector<std::string>  values = cmdOpt.getValue();
         tester.assert( (values.size() == 1), "Expected CommandOption to have 1 value.", __LINE__ );
         if (values.size() == 1)
         {
            tester.assert( (values[0].compare("value") == 0), "Expected CommandOption value to be 'value'.", __LINE__ );
         }
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   try  // Parse with an unexpected standard CommandOption
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType, 'f', "foo", "Foo", false, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 2;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-g") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser should have generated errors while parsing.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   try  // Parse with an unexpected trailing CommandOption
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType, 'f', "foo", "Foo", false, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 3;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f"), const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser should have generated errors while parsing.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   try  // Parse with a missing required CommandOption
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType, 'f', "foo", "Foo", true, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 2;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("trailing") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser should have generated errors while parsing.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   try  // Parse with a violated CommandOption max count
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType, 'f', "foo", "Foo", false, testCmdOptVec);
      cmdOpt.setMaxCount(1);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 3;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f"), const_cast<char*>("-f") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser should have generated errors while parsing.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   try  // Parse with multiple CommandOptions
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt1(CommandOption::noArgument, CommandOption::stdType, 'f', "foo", "Foo", false, testCmdOptVec);
      CommandOption  cmdOpt2(CommandOption::noArgument, CommandOption::stdType, 'g', "goo", "Goo", false, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 3;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-g"), const_cast<char*>("-f") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         std::ostringstream  count1Oss;
         count1Oss << cmdOpt1.getCount();         
         tester.assert( (cmdOpt1.getCount() == 1), "Expected 1 instance of CommandOption 1, not " + count1Oss.str(), __LINE__ );
         std::ostringstream  count2Oss;
         count2Oss << cmdOpt1.getCount();         
         tester.assert( (cmdOpt2.getCount() == 1), "Expected 1 instance of CommandOption 2, not " + count2Oss.str(), __LINE__ );
         std::ostringstream  order1Oss;
         order1Oss << cmdOpt2.getOrder();
         tester.assert( (cmdOpt1.getOrder() == 2), "Expected CommandOption 1 to be option 2, not " + order1Oss.str(), __LINE__ );
         std::ostringstream  order2Oss;
         order2Oss << cmdOpt2.getOrder();
         tester.assert( (cmdOpt2.getOrder() == 1), "Expected CommandOption 2 to be option 1, not " + order2Oss.str(), __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   try  // Parse a CommandOption with no short option
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType, 0, "foo", "Foo", false, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 2;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("--foo") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         tester.assert( (cmdOpt.getCount() == 1), "Expected 1 instance of CommandOption, not " + countOss.str(), __LINE__ );
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         tester.assert( (cmdOpt.getOrder() == 1), "Expected CommandOption to be option 1, not " + orderOss.str(), __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   try  // Parse a CommandOption with no long option
   {
      CommandOptionVec  testCmdOptVec;
      CommandOption  cmdOpt(CommandOption::noArgument, CommandOption::stdType, 'f', "", "Foo", false, testCmdOptVec);
      CommandOptionParser  cop("Description", testCmdOptVec);
      int  argc = 2;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         tester.assert( (cmdOpt.getCount() == 1), "Expected 1 instance of CommandOption, not " + countOss.str(), __LINE__ );
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         tester.assert( (cmdOpt.getOrder() == 1), "Expected CommandOption to be option 1, not " + orderOss.str(), __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionNoArg
   {
      CommandOptionNoArg  cmdOpt('f', "foo", "Foo", false);
      CommandOptionParser  cop("Description");
      int  argc = 3;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f"), const_cast<char*>("--foo") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         tester.assert( (cmdOpt.getCount() == 2), "Expected 2 instances of CommandOptionNoArg, not " + countOss.str(), __LINE__ );
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         tester.assert( (cmdOpt.getOrder() == 2), "Expected CommandOptioNoArg to be option 2, not " + orderOss.str(), __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithArg
   {
      CommandOptionWithArg  cmdOpt(CommandOption::stdType, 'f', "foo", "Foo", false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f"), const_cast<char*>("value1"), const_cast<char*>("--foo"), const_cast<char*>("value2") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         tester.assert( (cmdOpt.getCount() == 2), "Expected 2 instances of CommandOptionWithArg, not " + countOss.str(), __LINE__ );
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         tester.assert( (cmdOpt.getOrder() == 2), "Expected CommandOptioWithArg to be option 2, not " + orderOss.str(), __LINE__ );
         std::vector<std::string>  values = cmdOpt.getValue();
         tester.assert( (values.size() == 2), "Expected CommandOption to have 2 values.", __LINE__ );
         if (values.size() == 2)
         {
            tester.assert( (values[0].compare("value1") == 0), "Expected 1st CommandOptionWithArg value to be 'value1'.", __LINE__ );
            tester.assert( (values[1].compare("value2") == 0), "Expected 2nd CommandOptionWithArg value to be 'value2'.", __LINE__ );
         }
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithAnyArg
   {
      CommandOptionWithAnyArg  cmdOpt('f', "foo", "Foo", false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f"), const_cast<char*>("value1"), const_cast<char*>("--foo"), const_cast<char*>("value2") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         tester.assert( (cmdOpt.getCount() == 2), "Expected 2 instances of CommandOptionWithAnyArg, not " + countOss.str(), __LINE__ );
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         tester.assert( (cmdOpt.getOrder() == 2), "Expected CommandOptioWithAnyArg to be option 2, not " + orderOss.str(), __LINE__ );
         std::vector<std::string>  values = cmdOpt.getValue();
         tester.assert( (values.size() == 2), "Expected CommandOption to have 2 values.", __LINE__ );
         if (values.size() == 2)
         {
            tester.assert( (values[0].compare("value1") == 0), "Expected 1st CommandOptionWithAnyArg value to be 'value1'.", __LINE__ );
            tester.assert( (values[1].compare("value2") == 0), "Expected 2nd CommandOptionWithAnyArg value to be 'value2'.", __LINE__ );
         }
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered expected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( true, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( false, "CommandOptionParser parsed without errors but should have rejected the argument value due to its format", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithStringArg (valid)
   {
      CommandOptionWithStringArg  cmdOpt('f', "foo", "Foo", false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("-f"), const_cast<char*>("valueOne"), const_cast<char*>("--foo"), const_cast<char*>("valueTwo") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         tester.assert( (cmdOpt.getCount() == 2), "Expected 2 instances of CommandOptionWithStringArg, not " + countOss.str(), __LINE__ );
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         tester.assert( (cmdOpt.getOrder() == 2), "Expected CommandOptioWithStringArg to be option 2, not " + orderOss.str(), __LINE__ );
         std::vector<std::string>  values = cmdOpt.getValue();
         tester.assert( (values.size() == 2), "Expected CommandOption to have 2 values.", __LINE__ );
         if (values.size() == 2)
         {
            tester.assert( (values[0].compare("valueOne") == 0), "Expected 1st CommandOptionWithStringArg value to be 'valueOne'.", __LINE__ );
            tester.assert( (values[1].compare("valueTwo") == 0), "Expected 2nd CommandOptionWithStringArg value to be 'valueTwo'.", __LINE__ );
         }
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered expected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( true, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( false, "CommandOptionParser parsed without errors but should have rejected the argument value due to its format", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         tester.assert( (cmdOpt.getCount() == 2), "Expected 2 instances of CommandOptionWithNumberArg, not " + countOss.str(), __LINE__ );
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         tester.assert( (cmdOpt.getOrder() == 2), "Expected CommandOptioWithNumberArg to be option 2, not " + orderOss.str(), __LINE__ );
         std::vector<std::string>  values = cmdOpt.getValue();
         tester.assert( (values.size() == 2), "Expected CommandOption to have 2 values.", __LINE__ );
         if (values.size() == 2)
         {
            tester.assert( (values[0].compare("0") == 0), "Expected 1st CommandOptionWithNumberArg value to be '0'.", __LINE__ );
            tester.assert( (values[1].compare("12345") == 0), "Expected 2nd CommandOptionWithNumberArg value to be '12345'.", __LINE__ );
         }
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered expected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( true, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( false, "CommandOptionParser parsed without errors but should have rejected the argument value due to its format", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         tester.assert( (cmdOpt.getCount() == 2), "Expected 2 instances of CommandOptionWithDecimalArg, not " + countOss.str(), __LINE__ );
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         tester.assert( (cmdOpt.getOrder() == 2), "Expected CommandOptionWithDecimalArg to be option 2, not " + orderOss.str(), __LINE__ );
         std::vector<std::string>  values = cmdOpt.getValue();
         tester.assert( (values.size() == 2), "Expected CommandOption to have 2 values.", __LINE__ );
         if (values.size() == 2)
         {
            tester.assert( (values[0].compare("0") == 0), "Expected 1st CommandOptionWithDecimalArg value to be '0'.", __LINE__ );
            tester.assert( (values[1].compare("123.45") == 0), "Expected 2nd CommandOptionWithDecimalArg value to be '123.45'.", __LINE__ );
         }
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithCommonTimeArg (invalid)
   {
      CommandOptionWithCommonTimeArg  cmdOpt('t', "time", "%Y %j %s", "Time", false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-t"),
                        const_cast<char*>("value"),
                        const_cast<char*>("--time"),
                        const_cast<char*>("1234") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered expected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( true, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( false, "CommandOptionParser parsed without errors but should have rejected the argument value due to its format.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionWithCommonTimeArg (valid YDS)
   {
      CommandOptionWithCommonTimeArg  cmdOpt('t', "time", "%Y %j %s", "Time", false);
      CommandOptionParser  cop("Description");
      int  argc = 5;
      char*  argv[] = { const_cast<char*>("program"),
                        const_cast<char*>("-t"),
                        const_cast<char*>("2015 123 45678.0"),
                        const_cast<char*>("--time"),
                        const_cast<char*>("2015 234 56789.0") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         tester.assert( (cmdOpt.getCount() == 2), "Expected 2 instances of CommandOptionWithCommonTimeArg, not " + countOss.str(), __LINE__ );
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         tester.assert( (cmdOpt.getOrder() == 2), "Expected CommandOptionWithCommonTimeArg to be option 2, not " + orderOss.str(), __LINE__ );
         std::vector<std::string>  values = cmdOpt.getValue();
         tester.assert( (values.size() == 2), "Expected CommandOption to have 2 values.", __LINE__ );
         if (values.size() == 2)
         {
            tester.assert( (values[0].compare("2015 123 45678.0") == 0), "Expected 1st CommandOptionWithCommonTimeArg value to be '2015 123 45678.0'.", __LINE__ );
            tester.assert( (values[1].compare("2015 234 56789.0") == 0), "Expected 2nd CommandOptionWithCommonTimeArg value to be '2015 234 56789.0'.", __LINE__ );
         }
         std::vector<gpstk::CommonTime>  times = cmdOpt.getTime();
         tester.assert( (times.size() == 2), "Expected CommandOption to have 2 times.", __LINE__ );
         if (times.size() == 2)
         {
            gpstk::CommonTime  t1 = gpstk::YDSTime(2015, 123, 45678.0).convertToCommonTime();
            gpstk::CommonTime  t2 = gpstk::YDSTime(2015, 234, 56789.0).convertToCommonTime();
            tester.assert( (times[0] == t1), "Expected 1st CommandOptionWithCommonTimeArg time to be '2015 123 45678.0'.", __LINE__ );
            tester.assert( (times[1] == t2), "Expected 2nd CommandOptionWithCommonTimeArg time to be '2015 234 56789.0'.", __LINE__ );
         }
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   try  // Parse with a missing required CommandOptionRest
   {
      CommandOptionRest  cmdOpt("Description", true);
      CommandOptionParser  cop("Description");
      int  argc = 1;
      char*  argv[] = { const_cast<char*>("program") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser should have produced errors due to missing CommandOptionRest.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   try  // Parse a CommandOptionRest
   {
      CommandOptionRest  cmdOpt("Description", false);
      CommandOptionParser  cop("Description");
      int  argc = 3;
      char*  argv[] = { const_cast<char*>("program"), const_cast<char*>("trailing1"), const_cast<char*>("trailing2") };
      cop.parseOptions(argc, argv);
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         std::ostringstream  countOss;
         countOss << cmdOpt.getCount();
         tester.assert( (cmdOpt.getCount() == 2), "Expected 2 instances of CommandOptionRest, not " + countOss.str(), __LINE__ );
         std::ostringstream  orderOss;
         orderOss << cmdOpt.getOrder();
         tester.assert( (cmdOpt.getOrder() == 2), "Expected CommandOptionRest to be option 2, not " + orderOss.str(), __LINE__ );
         std::vector<std::string>  values = cmdOpt.getValue();
         tester.assert( (values.size() == 2), "Expected CommandOption to have 2 values.", __LINE__ );
         if (values.size() == 2)
         {
            tester.assert( (values[0].compare("trailing1") == 0), "Expected 1st CommandOptionRest value to be 'trailing1'.", __LINE__ );
            tester.assert( (values[1].compare("trailing2") == 0), "Expected 1st CommandOptionRest value to be 'trailing2'.", __LINE__ );
         }
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   return tester.countFails();
}


/*************************************************************************
 */
int CommandOptionParser_T::testOptionPresence()
{
   TestUtil  tester( "CommandOptionParser", "OptionPresence", __FILE__, __LINE__ );

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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         tester.assert( com.whichOne() == &cmdOptF, "whichOne() reported an unexpected option.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser parsed without errors but should have reported a violated mutex.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser parsed without errors but should have reported a violated mutex.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser parsed without errors but should have reported a missing dependency", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser parsed without errors but should have reported a missing dependency", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         // @todo - Check which() result
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         // @todo - Check which() result
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         // @todo - Check which() result
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         // @todo - Check which() result
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser parsed without errors but should have reported an excessive argument.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser parsed without errors but should have reported an excessive argument.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser parsed without errors but should have reported too few arguments.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         tester.assert( cooo.whichOne() == &cmdOptB, "whichOne() reported an unexpected option.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         tester.assert( cooo.whichOne() == &cmdOptF, "whichOne() reported an unexpected option.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser parsed without errors but should have reported a missing argument.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser parsed without errors but should have reported a missing argument.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         tester.assert( cogo.whichOne() == &cmdOptF, "whichOne() reported an unexpected option.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser parsed without errors but should have reported a missing dependency.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      if (cop.hasErrors() )
      {
         std::ostringstream  oss;
         oss << "CommandOptionParser encountered unexpected errors while parsing: ";
         cop.dumpErrors(oss);
         tester.assert( false, oss.str(), __LINE__ );
      }
      else
      {
         tester.assert( true, "CommandOptionParser parsed without errors.", __LINE__ );
         tester.assert( coga.whichOne() == &cmdOptF, "whichOne() reported an unexpected option.", __LINE__ );
      }
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
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
      tester.assert( true, "CommandOptionParser parsed the options without throwing an exception.", __LINE__ );
      tester.assert( cop.hasErrors(), "CommandOptionParser parsed without errors but should have reported a missing dependency.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionParser() threw an exception while parsing but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();
#endif

   return tester.countFails();
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

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal << std::endl;

   return( errorTotal );
   
}  // main()
