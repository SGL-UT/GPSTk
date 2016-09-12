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
   cout << "Entering SVNumXRefDUMP" << endl;

   SVNumXRef svNumXRef;
   svNumXRef.dump(cout);
  
   return(0);
}
