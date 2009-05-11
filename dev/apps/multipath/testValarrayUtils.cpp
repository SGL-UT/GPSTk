#pragma ident "$Id$"
#include "ValarrayUtils.hpp"

#include <iostream>
#include <set>

using namespace std;
using namespace gpstk;
using namespace ValarrayUtils;

int main(void)
{
   int theSet[]= { 1, 2, 2, 2, 3, 4, 4, 5};
   valarray<int> v1(theSet,8);

   cout << "Original: " << endl << v1 << endl;

   set<int> s1=unique(v1);

   cout << "Unique:" << endl << s1 << endl;
          
   cout << "End of test." << endl;
   return 0;
}

   
