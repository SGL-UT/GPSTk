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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

/**
 * @file sp32bc.cpp
 * Reads SP3 format precise ephemeris file, outputs a series of RINEX ephemeris  * One ephemeris is generated for each satellite and epoch.
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "RinexNavStream.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "EphemerisStore.hpp"
#include "BCEphemerisStore.hpp"
#include "SP3Stream.hpp"
#include "SP3Header.hpp"
#include "SP3Data.hpp"
#include "DayTime.hpp"
#include "SatID.hpp"
#include "StringUtils.hpp"
#include "CommandOptionParser.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char *argv[])
{
   try {
      CommandOptionNoArg helpOption('h',"help","Display argument list.",false);
     CommandOptionWithStringArg sp3input('p',"pe","Input precise ephemeris");
     CommandOptionWithNumberArg rate('r', "output-rate", "Rate of broadcast ephemeris output (seconds)");
      CommandOptionParser cop("GPSTk SP3 to RINEX NAV Conversion");
      cop.parseOptions(argc, argv);

      if (cop.hasErrors())
      {
         cop.dumpErrors(cout);
         cop.displayUsage(cout);
         return 1;
      }

      if(helpOption.getCount())
      {
         cop.displayUsage(cout);
         return 0;
      }


   }
   catch( Exception error)
   {
      cout << error << endl;
      exit(-1);
   }

   exit(0);

}
