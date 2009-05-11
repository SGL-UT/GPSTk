#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//============================================================================

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

      RinexObsData::RinexSatMap::const_iterator it;

      for (it = rod.obs.begin(); it!= rod.obs.end(); it++)
      {
          RinexObsData::RinexObsTypeMap otmap;

          cout << rod.time.printf("%F %g ");
          cout << it->first << " ";
          cout << setprecision(5) << c[it->first];
          cout << endl;  
      }
      
   }   // Loop through observations
   

   exit(0);   
}
