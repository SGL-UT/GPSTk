#pragma ident "$Id: GloFreqNo.cpp 1709 2009-02-18 20:27:47Z btolman $"

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
// This software developed by Applied Research Laboratories at the University
// of Texas at Austin, under contract to an agency or agencies within the U.S. 
// Department of Defense. The U.S. Government retains all rights to use,
// duplicate, distribute, disclose, or release this software. 
//
// Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

//
// Tests the GLONASS frequency number singleton.
//

#include <iostream>
#include <iomanip>

//#include "icd_glo_constants.hpp"
#include "GloFreqIndex.hpp"

using namespace std;
using namespace gpstk;

int main()
{
//   vector<double> dum1, dum2;
   int ierr1, ierr2, ierr3;
   ierr1 = 0;
   ierr2 = 0;

   GloFreqIndex glo;
   glo.knownIndex(); // For testing, fill map with known frequency index data.

   cout << "SVID  index   G1         err   G2         err   G3         err" << endl;

   for (int i = 1; i <= 24; i++)
   {
      RinexSatID id = RinexSatID(i,SatID::systemGlonass);

      double freq1 = glo.getGloFreq(id,1,ierr1);
      double freq2 = glo.getGloFreq(id,2,ierr2);
      double freq3 = glo.getGloFreq(id,3,ierr3);

      cout << "R" << setw(2) << setfill('0') << i << "   "
           << setw(4) << setfill(' ') << glo.getGloIndex(id) << "    "
           << fixed << setprecision(4) << setw(9) << setfill(' ')
           << freq1 << "  " << ierr1 << "     "
           << fixed << setprecision(4) << setw(9) << setfill(' ')
           << freq2 << "  " << ierr2 << "     "
           << fixed << setprecision(4) << setw(9) << setfill(' ')
           << freq3 << "  " << ierr3 << endl;
   }

   exit(0);
}

