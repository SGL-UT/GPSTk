//============================================================================
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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

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


int main(int argc, char *argv[])
{
   using namespace gpstk;
   using namespace std;

   map< ObsID, string> i2s;

   i2s[ObsID("C1C")] = "C1C";
   i2s[ObsID("L1C")] = "L1C";
   i2s[ObsID("D1C")] = "D1C";
   i2s[ObsID("S1C")] = "S1C";
   i2s[ObsID("L1W")] = "L1W";
   i2s[ObsID("D2Z")] = "D2Z";
   i2s[ObsID("S1X")] = "S1X";
   i2s[ObsID("L5Q")] = "L5Q";
   i2s[ObsID("EL5Q")] = "EL5Q";
   i2s[ObsID("EC1C")] = "EC1C";
   i2s[ObsID("C4x")] = "C4x";  // Note that you can just start using custom id's
   i2s[ObsID("C4y")] = "C4y";

   // You can also explicitly create one so it can have a good description
   ObsID dfif = ObsID::newID("C3 ", "Ionosphere-free pseudorange");
   i2s[dfif] = "C3 ";

   // You can also fix up the descriptions manually
   ObsID C4y("C4y");
   ObsID::tcDesc[C4y.code] = "y";
   ObsID::cbDesc[C4y.band] = "L4";

   ObsID l1lc(ObsID::otTrackLen, ObsID::cbL1, ObsID::tcY);
   i2s[l1lc] = "cust";

   i2s[ObsID("C1 ")] = "C1 ";

   try
   {
      ObsID garbage = ObsID::newID("C1C", "this should fail");
      cout << "Error:" << StringUtils::asString(garbage) << " didn't fail " << garbage << endl;
   }
   catch (Exception& e)
   { cout << e << endl; }

   // Dump the map of obs ids
   cout << "Rinex   Ctor   Description" << endl;
   for (map<ObsID,string>::const_iterator i=i2s.begin(); i != i2s.end(); i++)
      cout << left << setw(7) << RinexObsID(i->first).asString()
           << " " << setw(7) << i->second
           << " " << i->first << endl;
}
