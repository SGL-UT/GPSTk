#include <iostream>
#include "LinearCombination.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
   if (argc !=2)
   {
      cerr << "Usage: "  << endl;
      cerr << "  " << argv[0] << " <RINEX obs filename> " << endl;
      exit(-1);
   }
   
   RinexObsStream ros(argv[1]);
   RinexObsData rod;
   
   while (ros >> rod)
   {
      
      LinearCombination lc1("P1-P2");

      lc1.addCoefficient(RinexObsHeader::P1, 1);
      lc1.addCoefficient(RinexObsHeader::P2, -1);

      EpochCombination c=lc1.evaluate(rod);

      RinexObsData::RinexPrnMap::const_iterator it;

      for (it = rod.obs.begin(); it!= rod.obs.end(); it++)
      {
          RinexObsData::RinexObsTypeMap otmap;

          cout << rod.time.printf("%F %g ");
          cout << it->first.prn << " ";
          cout << setprecision(5) << c[it->first];
          cout << endl;  
      }
      
   }   // Loop through observations
   

   exit(0);   
}
