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

#include "SVNumXRef.hpp"

#include "TestUtil.hpp"

int main()
{
   TUDEF("SVNumXRef", "");
   
   gpstk::SVNumXRef svNumXRef;
   
   testFramework.changeSourceMethod("svNumXRef isConsistent");
   TUASSERTE(bool, true, svNumXRef.isConsistent());
  
   std::cout << "Total Failures for " << __FILE__ << ": " << testFramework.countFails() << std::endl;
   
   return testFramework.countFails();
}
