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
void CycleSlipList::purgeDuplicates()
{
   // First make sure the list is sorted.
   sort();
   return;
   DayTime t(DayTime::BEGINNING_OF_TIME);
   vector<int> svCount(MAX_PRN);
   for (const_iterator i=begin(); i!=end(); i++)
   {
      const CycleSlipRecord& cs=*i;
      if (t != cs.t)
      {
         for (int i=1; i<=MAX_PRN; i++)
            cout << svCount[i] << " ";
         cout << endl;
         
         for (int i=1; i<=MAX_PRN; i++)
            svCount[i] = 0;
         t = cs.t;
      }
      svCount[cs.sv1.id]++;
      svCount[cs.sv2.id]++;
   }
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void CycleSlipList::dump(std::ostream& s) const
{
   long l1=0, l2=0;
   for (const_iterator i=begin(); i!=end(); i++)
      if (i->oid.band == ObsID::cbL1)
         l1++;
      else if (i->oid.band == ObsID::cbL2)
         l2++;
         
   s << "#  Total Cycle slips: " << size() << endl
     << "#  Cycle slips on L1: " << l1 << endl
     << "#  Cycle slips on L2: " << l2 << endl;

   if (size() == 0)
      return;

   s << endl
     << "#   time                sv1 sv2 obs type            cyles       el1    el2     pre   post    gap "
     << endl;

   s.setf(ios::fixed, ios::floatfield);
   
   for (const_iterator i=begin(); i!=end(); i++)
   {
      const CycleSlipRecord& cs=*i;
      string time=cs.t.printf("%4Y %3j %02H:%02M:%04.1f");
      
      s << ">c " << left << setw(20) << time
        << "  " << right << setw(2) << cs.sv1.id
        << "  " << right << setw(2) << cs.sv2.id
        << " " << left << setw(14) << StringUtils::asString(cs.oid) << right
        << " " << setprecision(3) << setw(14) << cs.cycles
        << "  " << std::setprecision(2) << setw(5) << cs.el1
        << "  " << std::setprecision(2) << setw(5) << cs.el2
        << "  " << setw(5) << cs.preCount
        << "  " << setw(5) << cs.postCount
        << "  " << setw(5) << setprecision(1) << cs.preGap
        << endl;
   }
   s << endl;
}
