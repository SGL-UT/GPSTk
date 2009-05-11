#pragma ident "$Id$"
#include <iostream>
#include <iomanip>

#include "RinexObsBase.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{
   // Create the input file stream
   RinexObsStream rin("bahr1620.04o");

   // Create the output file stream
   RinexObsStream rout("bahr1620.04o.new", ios::out|ios::trunc);

   // Read the RINEX header
   RinexObsHeader head; //RINEX header object
   rin >> head;
   rout.header = rin.header;
   rout << rout.header;

   // Loop over all data epochs
   RinexObsData data; //RINEX data object
   while (rin >> data) {
     rout << data;
   }

   exit(0);
}
