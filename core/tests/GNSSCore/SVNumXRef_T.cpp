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
//============================================================================
 /*********************************************************************
*
*  Test program for gpstk/core/lib/GNSSCore/SVNumXRef.
*  The last line of output will calculate how many tests fail.
*
*********************************************************************/
#include <iostream>
#include <cmath>

#include "CivilTime.hpp"
#include "SVNumXRef.hpp"

#include "TestUtil.hpp"

using namespace gpstk;

int main()
{
   TUDEF("SVNumXRef", "");
   
   SVNumXRef svNumXRef;
   
   //Test for overlap in SVN/PRN active time range(s)
   //output 0 for pass; output 1 for any fail
   testFramework.changeSourceMethod(" isConsistent");
   TUASSERTE(bool, true, svNumXRef.isConsistent());
   
   
   //Test that correct SVN availablility is given at specific time, given PRN
   //first statement intended to be true; second intended to be false
   testFramework.changeSourceMethod(" NAVSTARIDAvailable");
   TUASSERTE(bool, true, svNumXRef.NAVSTARIDAvailable(8, 
			  CivilTime(2015, 10, 1, 0, 0, 0.0, TimeSystem::GPS)));
   TUASSERTE(bool, false, svNumXRef.NAVSTARIDAvailable(8, 
                          CivilTime(2015, 7, 14, 0, 0, 0.0, TimeSystem::GPS)));
   
   
   //Test that correct SVN is returned, given PRN and active time range
   //try-catch ensures that an exception is thrown for a nonexistent PRN                
   testFramework.changeSourceMethod(" getNAVSTAR");
   TUASSERTE(int, 72, svNumXRef.getNAVSTAR(8, 
                       CivilTime(2015, 10, 1, 0, 0, 0.0, TimeSystem::GPS)));
   try
   {
     svNumXRef.getNAVSTAR(0, 
		CivilTime(2015, 10, 1, 0, 0, 0.0, TimeSystem::GPS));
     TUFAIL("getNAVSTAR(0) should have failed");
   }
   catch( NoNAVSTARNumberFound )
   {
     TUPASS("");
   }
   
      
   //Test that SV is correctly displayed as active, given SVN and active time range
   //first statement intended to be true; second intended to be false                      
   testFramework.changeSourceMethod(" NAVSTARIDActive");
   TUASSERTE(bool, true, svNumXRef.NAVSTARIDActive(72, 
			  CivilTime(2015, 10, 1, 0, 0, 0.0, TimeSystem::GPS)));
   TUASSERTE(bool, false, svNumXRef.NAVSTARIDActive(72, 
                          CivilTime(2015, 7, 14, 0, 0, 0.0, TimeSystem::GPS)));
   
   
   //Test that exception is thrown if BlockType cannot be located for SVN input
   //try-catch ensures that an exception is thrown for a nonexistent SVN
   testFramework.changeSourceMethod(" getBlockType");
   TUASSERTE(SVNumXRef::BlockType, SVNumXRef::IIF,
	     svNumXRef.getBlockType(72));
   try
   {
     svNumXRef.getBlockType(0);
     TUFAIL("getBlockType(0) should have failed");
   }
   catch( NoNAVSTARNumberFound )
   {
     TUPASS("");
   }
								    
                          
   //Test string output for corresponding block type or unknown if not found                      
   testFramework.changeSourceMethod(" getBlockTypeString");
   TUASSERTE(std::string, "Block IIF", svNumXRef.getBlockTypeString(72));
   TUASSERTE(std::string, "unknown", svNumXRef.getBlockTypeString(0));
   
   
   //Test that correct PRN is returned, given SVN and active time range
   testFramework.changeSourceMethod(" getPRNID");
   TUASSERTE(int, 8, svNumXRef.getPRNID(72, 
		      CivilTime(2015, 10, 1, 0, 0, 0.0, TimeSystem::GPS)));
   try
   {
     svNumXRef.getPRNID(0, 
		CivilTime(2015, 10, 1, 0, 0, 0.0, TimeSystem::GPS));
     TUFAIL("getPRNID(0) did not fail");
   }
   catch( NoNAVSTARNumberFound )
   {
     TUPASS("");
   }
   
   
   //Test that SV is correctly displayed as active, given PRN and active time range
   //first statement intended to be true; second intended to be falst                      
   testFramework.changeSourceMethod(" PRNIDAvailable");
   TUASSERTE(bool, true, svNumXRef.PRNIDAvailable(72, 
                          CivilTime(2015, 10, 1, 0, 0, 0.0, TimeSystem::GPS)));
   TUASSERTE(bool, false, svNumXRef.PRNIDAvailable(72, 
                          CivilTime(2015, 7, 14, 0, 0, 0.0, TimeSystem::GPS)));
   
   
   //Test that SVN corresponds to an available block type    
   //first statement intended to be true; second intended to be false                  
   testFramework.changeSourceMethod(" BlockTypeAvailable");
   TUASSERTE(bool, true, svNumXRef.BlockTypeAvailable(72));
   TUASSERTE(bool, false, svNumXRef.BlockTypeAvailable(0));                      
                         
                         
   std::cout << "Total Failures for " << __FILE__ << ": " << testFramework.countFails() << std::endl;
   
   
   return testFramework.countFails();
}
