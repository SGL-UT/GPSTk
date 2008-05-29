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
   // First make sure the list is sorted by time.
   sort();

   CycleSlipList purged;

   typedef map<SatID, int> SatInt;
   typedef map<ObsID, SatInt> ObsSatInt;
   for (iterator i=begin(); i!=end();)
   {
      ObsSatInt osi;
      iterator j;
      for (j=i; j != end() && j->t == i->t; j++)
      {
         osi[j->oid][j->sv1]++;
         osi[j->oid][j->sv2]++;
      }

      // Loop over each code/carrier combo that had a slip at this epoch
      for (ObsSatInt::const_iterator k=osi.begin(); k != osi.end(); k++)
      {
         const ObsID& oid = k->first;
         const SatInt& si = k->second;
         SatID sv;
         int cnt=0;

         // Try to guess which SV actually had the slip
         // No, this won't handle cycle slips from multiple SVs at a single epoch
         for (SatInt::const_iterator l=si.begin(); l != si.end(); l++)
         {
            // Record which SV was seen the most
            if (l->second > cnt)
            {
               cnt = l->second;
               sv = l->first;
            }
         }

         // Now select one entry to keep
         iterator keeper=end();
         for (iterator l=i; l != j && keeper==end(); l++)
         {
            if (l->oid != oid)
               continue;
            if (((cnt >1 && l->sv2 == sv) || cnt==1))
               keeper = l;
         }
         // If none were found, we have to look on sv1
         if (keeper==end())
         {
            for (iterator l=i; l != j && keeper==end(); l++)
            {
               if (l->oid != oid)
                  continue;
               if (cnt >1 && l->sv1 == sv)
                  keeper = l;
            }
            // Need to exchange sv1 & sv2 since we think this was a slip on sv1
            keeper->sv1 = keeper->sv2;
            keeper->sv2 = sv;
            double tmp = keeper->el1;
            keeper->el1 = keeper->el2;
            keeper->el2 = tmp;
            keeper->cycles = - keeper->cycles;
         }
         purged.push_back(*keeper);
      }
      i=j;
   }
   *this = purged;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void CycleSlipList::dump(std::ostream& s) const
{
   typedef map<ObsID, int> ObsInt;
   ObsInt oi;
   for (const_iterator i=begin(); i!=end(); i++)
      oi[i->oid]++;

   int tcs=0;
   for (ObsInt::const_iterator i=oi.begin(); i != oi.end(); i++)
   {
      tcs += i->second;
      s << "#  Cycle slips on " << i->first << ": " << i->second << endl;
   }

   s << "#  Total cycle slips: " << tcs << endl;

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
