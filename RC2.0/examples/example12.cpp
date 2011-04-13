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
#include "Exception.hpp"


int main(int argc, char *argv[])
{
   using namespace gpstk;
   using namespace std;

   map< ObsID, string> i2s;

   i2s["C1C"] = "C1C";
   i2s["L1C"] = "L1C";
   i2s["D1C"] = "D1C";
   i2s["S1C"] = "S1C";
   i2s["L1W"] = "L1W";
   i2s["D2Z"] = "D2Z";
   i2s["S1X"] = "S1X";
   i2s["L5Q"] = "L5Q";
   i2s["EL5Q"] = "EL5Q";
   i2s["EC1C"] = "EC1C";
   i2s["C4x"] = "C4x";  // Note that you can just start using custom id's
   i2s["C4y"] = "C4y";

   // You can also explicitly create one so it can have a good description
   ObsID dfif = ObsID::newID("C3 ", "Ionosphere-free pseudorange");
   i2s[dfif] = "C3 ";

   // You can also fix up the descriptions manually
   ObsID C4y("C4y");
   ObsID::tcDesc[C4y.code] = "y";
   ObsID::cbDesc[C4y.band] = "L4";

   ObsID l1lc(ObsID::otTrackLen, ObsID::cbL1, ObsID::tcY);
   i2s[l1lc] = "cust";

   i2s["C1 "] = "C1 ";

   try
   {
      ObsID garbage = ObsID::newID("C1C", "this should fail");
      cout << "Error:" << garbage.asRinex3ID() << " didn't fail " << garbage << endl;
   }
   catch (Exception& e)
   { cout << e << endl; }

   // Dump the map of obs ids
   cout << "Rinex   Ctor   Description" << endl;
   for (map<ObsID,string>::const_iterator i=i2s.begin(); i != i2s.end(); i++)
      cout << left << setw(7) << i->first.asRinex3ID()
           << " " << setw(7) << i->second
           << " " << i->first << endl;
}
