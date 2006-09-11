#include <iostream>
#include <set>

#include "ValarrayUtils.hpp"
#include "BinnedStats.hpp"

using namespace std;
using namespace gpstk;
using namespace ValarrayUtils;

int main(void)
{
   double mpSrc[]= { 0.9, -1.6, 1.8, -.5,
                     1.0,  1.1, -.5, -.3,
                    .3,   -.25, 0.4, .05  };

   valarray<double> mpVals(mpSrc,12);
   
   double elSrc[]= {   5,   6,   7,    8, 
                      11,  30,  34,   42,
                      50,  60,  70,   80  };

   valarray<double> elVals(elSrc,12);


   double elBinsSrc[] = { 0, 10, 45, 90 };
   valarray<double> elevationBinBoundaries ( elBinsSrc, 4);

   cout << "Bin boundaries: " << endl;
   cout << elevationBinBoundaries << endl;

   BinnedStats<double> mstats(elevationBinBoundaries);

   mstats.add(mpVals, elVals);
   
   
   cout << "MP data: " << endl << mpVals << endl;
   cout << "Elevation data: " << endl << elVals << endl;


   
   return 0;
}

   
