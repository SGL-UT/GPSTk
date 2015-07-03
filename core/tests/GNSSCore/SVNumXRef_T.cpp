/*********************************************************************
*
*  Test program for gpstk/core/lib/GNSSCore/SVNumXRef.
*  The last line of output will be "1" if the test passes,
*  and "0" if it fails. 
*  isConsistent() returns 1 if there are no overlaps in 
*  PRN-SVN relationships and 0 otherwise.
*
*********************************************************************/
#include <stdio.h>
#include "SVNumXRef.hpp"

using namespace std;
using namespace gpstk;

int main( int argc, char * argv[] )
{
   cout << "Entering SVNumXRef_T" << endl;
   int testVal;
   SVNumXRef svNumXRef;
   testVal = svNumXRef.isConsistent();
   if (testVal == 1)
      testVal = 0;
  
   return(testVal);
}
