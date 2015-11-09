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

#include "CommandOptionWithCommonTimeArg.hpp"
#include "CommandOptionParser.hpp"
#include "YDSTime.hpp"
#include "TestUtil.hpp"
#include <iostream>

using namespace std;
using namespace gpstk;

class CommandOptionWithCommonTimeArg_T
{
public:
   CommandOptionWithCommonTimeArg_T() { }
   ~CommandOptionWithCommonTimeArg_T() { }

   int testInitialization();
};


int CommandOptionWithCommonTimeArg_T::testInitialization()
{
   TestUtil  tester( "CommandOptionWithCommonTimeArg", "Initialization", __FILE__, __LINE__ );

   CommandOptionVec  testCmdOptVec;

   defaultCommandOptionList.clear();

   try  // Unset parameterss
   {
      CommandOptionWithCommonTimeArg  cmdOpt(0, "", "", "", false);
      tester.assert( false, "CommandOptionWithDecimalArg creation should have failed due to missing short and long options.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( true, "CommandOptionWithDecimalArg() threw an exception as expected.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   try
   {
      CommandOptionWithCommonTimeArg  cmdOpt('t', "time", "%Y %j %s", "Time", false);
      tester.assert( true, "CommandOptionWithCommonTimeArg was created successfully.", __LINE__ );
      tester.assert( (cmdOpt.getCount() == 0), "CommandOptionWithCommonTimeArg count should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getValue().size() == 0), "CommandOptionWithCommonTimeArg value size should be 0.", __LINE__ );
      tester.assert( (cmdOpt.getOrder() == 0), "CommandOptionWithCommonTimeArg order should be 0.", __LINE__ );
      tester.assert( (cmdOpt.checkArguments().size() == 0), "CommandOptionWithCommonTimeArg checkArguments() should return nothing.", __LINE__ );
      tester.assert( (defaultCommandOptionList.size() == 1), "CommandOptionWithCommonTimeArg was not added to the default list.", __LINE__ );
   }
   catch ( ... )
   {
      tester.assert( false, "CommandOptionWithCommonTimeArg() threw an exception but should not have.", __LINE__ );
   }

   defaultCommandOptionList.clear();

   return tester.countFails();
}


/** Run the program.
 *
 * @return Total error count for all tests
 */
int main(int argc, char *argv[])
{
   int  errorTotal = 0;

   CommandOptionWithCommonTimeArg_T  testClass;

   errorTotal += testClass.testInitialization();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal << std::endl;

   return( errorTotal );
   
}  // main()
