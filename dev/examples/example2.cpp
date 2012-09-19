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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include <iostream>
#include <iomanip>

#include "Rinex3ObsBase.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"
#include "Rinex3ObsStream.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{

      // Create the input file stream
   Rinex3ObsStream rin("bahr1620.04o");

      // Create the output file stream
   Rinex3ObsStream rout("bahr1620.04o.new", ios::out|ios::trunc);

      // Read the RINEX header
   Rinex3ObsHeader head;    //RINEX header object
   rin >> head;
   rout.header = rin.header;
   rout << rout.header;

      // Loop over all data epochs
   Rinex3ObsData data;   //RINEX data object
   while (rin >> data)
   {
     rout << data;
   }

   exit(0);

}
