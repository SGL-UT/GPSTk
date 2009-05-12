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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include "ValarrayUtils.hpp"

#include <iostream>
#include <set>

using namespace std;
using namespace gpstk;
using namespace ValarrayUtils;

int main(void)
{
   int theSet[]= { 1, 2, 2, 2, 3, 4, 4, 5};
   valarray<int> v1(theSet,8);

   cout << "Original: " << endl << v1 << endl;

   set<int> s1=unique(v1);

   cout << "Unique:" << endl << s1 << endl;
          
   cout << "End of test." << endl;
   return 0;
}

   
