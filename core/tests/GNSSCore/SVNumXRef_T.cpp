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
   
   testFramework.changeSourceMethod("svNumXRef isConsistent");
   TUASSERTE(bool, true, svNumXRef.isConsistent());
   
   testFramework.changeSourceMethod("svNumXRef NAVSTARIDAvailable");
   TUASSERTE(bool, true, svNumXRef.NAVSTARIDAvailable(8, 
                          CivilTime(2015, 10, 1, 0, 0, 0.0, TimeSystem::GPS)));
   TUASSERTE(bool, false, svNumXRef.NAVSTARIDAvailable(8, 
                          CivilTime(2015, 7, 14, 0, 0, 0.0, TimeSystem::GPS)));
                         
   std::cout << "Total Failures for " << __FILE__ << ": " << testFramework.countFails() << std::endl;
   
   return testFramework.countFails();
}
