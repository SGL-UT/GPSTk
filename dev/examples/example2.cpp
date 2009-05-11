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
