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

#include "CycleSlipList.hpp"

using namespace std;
using namespace gpstk;

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void dump(std::ostream& s, const CycleSlipList& csl)
{
   s << "#  Total Cycle slips: " << csl.size() << endl;

   CycleSlipList::const_iterator i;
   long l1=0, l2=0;

   for (i=csl.begin(); i!=csl.end(); i++)
      if (i->oid.band == ObsID::cbL1)
         l1++;
      else if (i->oid.band == ObsID::cbL2)
         l2++;
         
   s << "#  Cycle slips on L1: " << l1 << endl;
   s << "#  Cycle slips on L2: " << l2 << endl;

   if (csl.size() == 0)
      return;

   s << endl
     << "#   time                prn obs type            cyles       elev     pre   post    gap  mstr " << endl;

   s.setf(ios::fixed, ios::floatfield);
   
   for (i=csl.begin(); i!=csl.end(); i++)
   {
      const CycleSlipRecord& cs=*i;
      string time=cs.t.printf("%4Y %3j %02H:%02M:%04.1f");
      
      s << ">c " << left << setw(20) << time
        << "  " << right << setw(2) << cs.prn.id
        << " " << left << setw(14) << StringUtils::asString(cs.oid) << right
        << " " << setprecision(3) << setw(14) << cs.cycles
        << "  " << std::setprecision(2) << setw(5) << cs.elevation
        << "  " << setw(5) << cs.preCount
        << "  " << setw(5) << cs.postCount
        << "  " << setw(5) << setprecision(1) << cs.preGap
        << "  " << setw(2) << cs.masterPrn.id
        << endl;
   }
   s << endl;
}
