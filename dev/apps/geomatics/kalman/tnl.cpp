#pragma ident "$Id: $"

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

/**
 * @file tnl.cpp
 * Test the namelist class.
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "Namelist.hpp"

using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
int main(void)
{
   vector<string> vs;
   Namelist N,N1,N2,N3;

   N1 += string("Fred");
   N1 += string("Millie");
   N1 += string("Constipation");
   N1 += string("Serendipity");
   N1 += string("Harmony");
   vs.push_back(string("X comp"));
   vs.push_back(string("Y bias"));
   vs.push_back(string("Harmony"));
   vs.push_back(string("Z res"));
   N2 = Namelist(vs);
   N3 = N1 | N2;

   cout << " Namelist 1: " << N1 << endl;
   cout << " Namelist 2: " << N2 << endl;
   cout << " Namelist 3: " << N3 << endl;

   N = N1 ^ N2;
   cout << " N1 xor N2: " << N << endl;
   N = N1 | N2;
   cout << " N1 or N2: " << N << endl;
   N = N1 & N3;
   cout << " N1 and N3: " << N << endl;
   N -= (string("Fred"));
   cout << " N1 and N3 without Fred: " << N << endl;
   N.resize(6);
   cout << " Prev NL resized to 6: " << N << endl;
   N -= string("Millie");
   N -= string("Harmony");
   N -= string("Constipation");
   cout << " Prev NL without Millie and Harmony and Constipation\n  " << N << endl;
   N.resize(6);
   cout << " Prev NL resized to 6: " << N << endl;
   N.resize(7);
   N.setName(5,string("Twenty"));
   cout << " Prev NL resized to 7: " << N << endl;
   N.sort();
   cout << " Prev NL sorted: " << N << endl;
   cout << " print the elements of N2 using get : ";
   for(int i=0; i<N2.size(); i++) cout << " / " << N2.getName(i);
   cout << endl;

   cout << "End test prgm" << endl;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

