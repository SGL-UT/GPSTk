/*********************************************************************
*
*  Demo program for gpstk/core/lib/GNSSCore/SVNumXRef.
*
*********************************************************************/
#include <stdio.h>
#include "SVNumXRef.hpp"

using namespace std;
using namespace gpstk;

int main( int argc, char * argv[] )
{
   cout << "Entering xSVNumXRef" << endl;

   SVNumXRef svNumXRef;
   svNumXRef.dump(cout, true);
  
   return(0);
}
