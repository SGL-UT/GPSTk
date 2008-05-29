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

#include <algorithm>

#include "Stats.hpp"

#include "PhaseResidual.hpp"

namespace PhaseResidual
{
   int debugLevel=0;

   using namespace std;
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   void Obs::dump(ostream& s, int detail) const
   {
      s << " phase:" << phase11
        << " dd:" << dd
        << " td:" << td;
   }


//------------------------------------------------------------------------------
// Note that this really needs to be called before a pass of an SV gets broken
// into multiple arcs
//------------------------------------------------------------------------------
   void Arc::computeTD(void)
   {
      if (debugLevel>1)
         cout << "Computing Triple difference for "
              << sv1 << " - " << sv2 << " " << obsID << endl;
      iterator i = begin();
      while (i != end())
      {
         Obs& prev = i->second;
         const gpstk::DayTime& t0 = i->first;
         i++;
         if (i == end())
            break;
         Obs& curr = i->second;
         const gpstk::DayTime t1 = i->first;
         curr.td = (curr.dd - prev.dd)/(t1 - t0);
      }
   }


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   void Arc::debiasDD(double bias)
   {
      ddBias += bias;
      for (iterator i = begin(); i != end(); i++)
         i->second.dd -= bias;
   }


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   gpstk::Stats<double> Arc::statsDD(void) const
   {
      gpstk::Stats<double> stats;

      for (const_iterator i = begin(); i != end(); i++)
         stats.Add(i->second.dd);

      return stats;
   }


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   void Arc::dump(ostream& s, int detail) const
   {
      if (size() == 0)
      {
         s << "# Arc: empty" << endl;
         return;
      }
         
      gpstk::Stats<double> stats = statsDD();
      const gpstk::DayTime& t0=begin()->first;
      const gpstk::DayTime& t1=rbegin()->first;

      s << left
        << "# Arc: " << t0.printf("%02H:%02M:%04.1f")
        << " - "   << t1.printf("%02H:%02M:%04.1f")
        << " SVs:" << sv1.id << "-" <<sv2.id
        << " " << obsID 
        << " N:" <<  setw(5) << stats.N() 
        << " bias:"  << setprecision(12) << ddBias;

      // Only output the standard deviation if it is valid
      if (stats.N()>1)
         s << " sdev:"  << setprecision(4) << stats.StdDev();        

      // Only output the average if it is statistically non-zero
      bool zero = stats.Average() <= stats.StdDev()/sqrt((float)stats.N());
      if (!zero)
         s << " avg:" << setprecision(4) << stats.Average();
      s << endl;

      if ((!zero || stats.StdDev()>0.5) && detail)
         for (const_iterator i=begin(); i != end(); i++)
            s << "# " << i->first.printf("%02H:%02M:%04.1f")
              << " " << i->second << endl;
   }


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   class timeGap
   {
   public:
      timeGap(double len):minGap(len){};
      double minGap;
      bool operator()(Arc::value_type& obs0, Arc::value_type& obs1)
      {
         return std::abs(obs0.first - obs1.first) > minGap;
      }
   };


//------------------------------------------------------------------------------
// Break the arc up in sections that are separated by the indicated number 
// of seconds.
//------------------------------------------------------------------------------
   void ArcList::splitOnGaps(double gapSize)
   {
      if (debugLevel>1)
         cout << "Splitting on gaps " 
              << begin()->sv1 << " - " << begin()->sv2
              << " " << begin()->obsID << endl;

      for (iterator arc = begin(); arc != end(); arc++)
      {
         Arc::iterator i = 
            adjacent_find(arc->begin(), arc->end(), timeGap(gapSize));

         if (i == arc->end())
            continue;

         // Make a new empty arc immedietly after the current arc and
         // move all the data from the break to the end of the current 
         // arc into the new arc.
         iterator nextArc = arc;
         nextArc++;
         nextArc = insert(nextArc, Arc());
         nextArc->sv1 = arc->sv1;
         nextArc->sv2 = arc->sv2;
         nextArc->obsID = arc->obsID;
         nextArc->ddBias = arc->ddBias;
         nextArc->insert(++i, arc->end());
         arc->erase(i, arc->end());
      }
   }


//------------------------------------------------------------------------------
// Just compute the triple difference for all arcs
//------------------------------------------------------------------------------
   void ArcList::computeTD(void)
   {
      for (iterator i=begin(); i!=end(); i++)
         i->computeTD();
   }


//------------------------------------------------------------------------------
// Break the arc on big triple differences
//------------------------------------------------------------------------------
   void ArcList::splitOnTD(double threshold)
   {
      if (debugLevel>1)
         cout << "Splitting on TD " 
              << begin()->sv1 << " - " << begin()->sv2
              << " " << begin()->obsID << endl;

      for (iterator arc = begin(); arc != end(); arc++)
      {
         for (Arc::iterator i = arc->begin(); i != arc->end(); i++)
         {
            Obs& pr = i->second;
            bool jump = std::abs(pr.td) > threshold;

            if (!jump)
               continue;

            // If the double difference returns to a similiar value
            // withing several epochs, treat this as noise, not a real jump.
            Arc::iterator j = i; j++;
            for (int n=0; n<4 && jump && j != arc->end(); n++)
            {
               double quad = std::abs(pr.td + j->second.td);
               if (quad < threshold)
                  jump = false;
            }
            
            if (jump)
            {
               // Make a new empty arc immedietly after the current arc and
               // move all the data from here to the end of the current arc into
               // the new arc.
               pr.td = 0;
               iterator nextArc = arc;
               nextArc++;
               nextArc = insert(nextArc, Arc());

               nextArc->sv1 = arc->sv1;
               nextArc->sv2 = arc->sv2;
               nextArc->obsID = arc->obsID;
               nextArc->ddBias = arc->ddBias;
               nextArc->insert(i, arc->end());
               arc->erase(i, arc->end());
               arc = nextArc;
               i = arc->begin();
            }
         }
      }
   }


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   void ArcList::debiasDD(void)
   {
      for (iterator i = begin(); i != end(); i++)
         i->debiasDD(i->statsDD().Average());
   }


//------------------------------------------------------------------------------
// Find the specified obs
//------------------------------------------------------------------------------
   bool ArcList::findObs(const gpstk::DayTime& t, Arc::const_iterator& obs)
   {
      for (iterator i = begin(); i != end(); i++)
      {
         obs = i->find(t);
         if (obs != i->end())
            return true;
      }
      return false;
   }


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   void ArcList::mergeArcs(
      long arcLen,
      double arcTime,
      double gapTime,
      double threshold)
   {
      if (debugLevel>1)
         cout << "Merging arcs " 
              << begin()->sv1 << " - " << begin()->sv2
              << " " << begin()->obsID << endl;

      for(iterator i = begin(); i != end(); )
      {
         Arc& prev = *i;
         i++;
         if (i == end())
            break;
         Arc& curr = *i;

         // Both arcs must have the same SVs
         if (curr.sv1 != prev.sv1 || curr.sv2 != prev.sv2)
            continue;

         // And the arcs must be close enough together in time...
         const gpstk::DayTime prev_end = prev.rbegin()->first;
         const gpstk::DayTime curr_begin = curr.begin()->first;
         double dt = curr_begin - prev_end;
         if (dt >= gapTime)
            continue;

         // Now get the standard deviations of each arc
         // and they are similiar relative to the threshold
         gpstk::Stats<double> prev_stats = prev.statsDD();
         gpstk::Stats<double> curr_stats = curr.statsDD();
         double curr_std = curr_stats.StdDev();
         double prev_std = prev_stats.StdDev();
         if (curr_std > threshold || prev_std > threshold)
            continue;

         // And the biases need to be close, relative to the stddev
         if (std::abs(curr.ddBias - prev.ddBias) > 2 * curr_std)
            continue;

         // Now the arcs can be merged..
         // First make the biases exactly the same
         curr.debiasDD(prev.ddBias - curr.ddBias);
         // Then put the data from the current arc into the previous
         prev.insert(curr.begin(), curr.end());
         // and kill the current arc
         i = erase(i);
         i--;
      }
   }


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   void ArcList::dump(ostream& s, int detail) const
   {
      gpstk::Stats<double> stats;
      for (const_iterator i = begin(); i != end(); i++)
         for (Arc::const_iterator j = i->begin(); j != i->end(); j++)
            stats.Add(j->second.dd);

      s << "# ArcList N:" << stats.N()
        << " sdev:" << setprecision(4) << stats.StdDev();

      // We only output the average is it is statistically non zero
      if (stats.Average() > stats.StdDev()/sqrt((float)stats.N()))
         s << " avg:" << setprecision(4) << stats.Average();

      s << endl;
      
      for (const_iterator i = begin(); i != end(); i++)
         i->dump(s, detail);

      s << "#" << endl;
   }

   ostream& operator << (ostream& s, const Obs& pr)
   { pr.dump(s); return s; }

   ostream& operator << (ostream& s, const Arc& arc)
   { arc.dump(s); return s; }

   ostream& operator << (ostream& s, const ArcList& pral)
   { pral.dump(s); return s; }

} // end of PhaseResidual namespace

