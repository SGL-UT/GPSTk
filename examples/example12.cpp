//==============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/*
  This is a example program to demonstrate some of the functionality of the
  ObsID class. The intent is to use ObsID as a key in a STL map of gps data. ObsID
  supports identifying the data in a manner that is similiar but can extend
  the Rinex 3 specification.
*/

#include <iostream>
#include <string>
#include <map>
#include <set>

#include "ObsID.hpp"
#include "RinexObsID.hpp"
#include "Exception.hpp"
#include "Rinex3ObsHeader.hpp"


int main(int argc, char *argv[])
{
   using namespace gpstk;
   using namespace std;

   map< RinexObsID, string> i2s;

      // abbreviate.
   double cv = gpstk::Rinex3ObsBase::currentVersion;
   i2s[RinexObsID("C1C", cv)] = "C1C";
   i2s[RinexObsID("L1C", cv)] = "L1C";
   i2s[RinexObsID("D1C", cv)] = "D1C";
   i2s[RinexObsID("S1C", cv)] = "S1C";
   i2s[RinexObsID("L1W", cv)] = "L1W";
   i2s[RinexObsID("D2Z", cv)] = "D2Z";
   i2s[RinexObsID("S1X", cv)] = "S1X";
   i2s[RinexObsID("L5Q", cv)] = "L5Q";
   i2s[RinexObsID("EL5Q", cv)] = "EL5Q";
   i2s[RinexObsID("EC1C", cv)] = "EC1C";
   i2s[RinexObsID("C4x", cv)] = "C4x";  // Note that you can just start using custom id's
   i2s[RinexObsID("C4y", cv)] = "C4y";

   // You can also explicitly create one so it can have a good description
   RinexObsID dfif = RinexObsID::newID("C3 ", "Ionosphere-free pseudorange");
   i2s[dfif] = "C3 ";

   // You can also fix up the descriptions manually
   RinexObsID C4y("C4y", cv);
   ObsID::tcDesc[C4y.code] = "y";
   ObsID::cbDesc[C4y.band] = "L4";

   RinexObsID l1lc(ObservationType::TrackLen, CarrierBand::L1, TrackingCode::Y);
   i2s[l1lc] = "cust";

   i2s[RinexObsID("C1 ", cv)] = "C1 ";

   try
   {
      RinexObsID garbage = RinexObsID::newID("C1C", "this should fail");
      cout << "Error:" << StringUtils::asString(garbage) << " didn't fail " << garbage << endl;
   }
   catch (Exception& e)
   { cout << e << endl; }

   // Dump the map of obs ids
   cout << "Rinex   Ctor   Description" << endl;
   for (map<RinexObsID,string>::const_iterator i=i2s.begin(); i != i2s.end(); i++)
      cout << left << setw(7) << RinexObsID(i->first).asString()
           << " " << setw(7) << i->second
           << " " << i->first << endl;
}
