/*********************************************************************
*
*  Test program for gpstk/core/lib/GNSSCore/SVNumXRef.
*  The last line of output will be "1" if the test passes,
*  and "0" if it fails. 
*  isConsistent() returns 1 if there are no overlaps in 
*  PRN-SVN relationships and 0 otherwise.
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
   testFramework.changeSourceMethod("svNumXRef isConsistent");
   TUASSERTE(bool, true, svNumXRef.isConsistent());
   
   
   //Test that correct SVN availablility is given at specific time, given PRN
   //first statement intended to be true; second intended to be false
   testFramework.changeSourceMethod("svNumXRef NAVSTARIDAvailable");
   TUASSERTE(bool, true, svNumXRef.NAVSTARIDAvailable(8, 
                          CivilTime(2015, 10, 1, 0, 0, 0.0, TimeSystem::GPS)));
   TUASSERTE(bool, false, svNumXRef.NAVSTARIDAvailable(8, 
                          CivilTime(2015, 7, 14, 0, 0, 0.0, TimeSystem::GPS)));
   
   
   //Test that correct SVN is returned, given PRN and active time range
   //try-catch ensures that an exception is thrown for a nonexistent PRN                
   testFramework.changeSourceMethod("svNumXRef getNAVSTAR");
   TUASSERTE(int, 72, svNumXRef.getNAVSTAR(8, 
                          CivilTime(2015, 10, 1, 0, 0, 0.0, TimeSystem::GPS)));
   try
   {
	   svNumXRef.getNAVSTAR(0, 
                          CivilTime(2015, 10, 1, 0, 0, 0.0, TimeSystem::GPS));
	   TUFAIL("getNAVSTAR(0) did not fail");
   }
   catch( NoNAVSTARNumberFound )
   {
	   TUPASS("");
   }
   
      
   //Test that SV is correctly displayed as active, given SVN and active time range
   //first statement intended to be true; second intended to be false                      
   testFramework.changeSourceMethod("svNumXRef NAVSTARIDActive");
   TUASSERTE(bool, true, svNumXRef.NAVSTARIDActive(72, 
                          CivilTime(2015, 10, 1, 0, 0, 0.0, TimeSystem::GPS)));
   TUASSERTE(bool, false, svNumXRef.NAVSTARIDActive(72, 
                          CivilTime(2015, 7, 14, 0, 0, 0.0, TimeSystem::GPS)));
   
   
   /*testFramework.changeSourceMethod("svNumXRef getBlockType");
   TUASSERTE(SVNumXRef::BlockType, SVNumXRef::BlockType::IIF,
								    svNumXRef.getBlockType(72));
   try
   {
	   svNumXRef.getBlockType(0);
	   TUFAIL("getBlockType(0) did not fail");
   }
   catch( NoNAVSTARNumberFound )
   {
	   TUPASS("");
   }*/
								    
                          
   //Tests string output for corresponding block type or unknown if not found                      
   testFramework.changeSourceMethod("svNumXRef getBlockTypeString");
   TUASSERTE(std::string, "Block IIF", svNumXRef.getBlockTypeString(72));
   TUASSERTE(std::string, "unknown", svNumXRef.getBlockTypeString(0));
   
   
   //Test that correct PRN is returned, given SVN and active time range
   testFramework.changeSourceMethod("svNumXRef getPRNID");
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
   testFramework.changeSourceMethod("svNumXRef PRNIDAvailable");
   TUASSERTE(bool, true, svNumXRef.PRNIDAvailable(72, 
                          CivilTime(2015, 10, 1, 0, 0, 0.0, TimeSystem::GPS)));
   TUASSERTE(bool, false, svNumXRef.PRNIDAvailable(72, 
                          CivilTime(2015, 7, 14, 0, 0, 0.0, TimeSystem::GPS)));
   
   
   //Test that SVN corresponds to an available block type    
   //first statement intended to be true; second intended to be false                  
   testFramework.changeSourceMethod("svNumXRef BlockTypeAvailable");
   TUASSERTE(bool, true, svNumXRef.BlockTypeAvailable(72));
   TUASSERTE(bool, false, svNumXRef.BlockTypeAvailable(0));                      
                         
                         
   std::cout << "Total Failures for " << __FILE__ << ": " << testFramework.countFails() << std::endl;
   
   
   return testFramework.countFails();
}
