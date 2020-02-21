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
//  Copyright 2006, The University of Texas at Austin
//
//============================================================================

#include "ScreenControl.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk 
{
   
   void clearScreen(void)
   {
      cout << "[H[J";
   }

   void printTitle(void)
   {
      cout << "GPSTk Real-Time Data Collection for the Ashtech Z-XII ver. 1.0" << endl;
   }
   
} // namespace gpstk
