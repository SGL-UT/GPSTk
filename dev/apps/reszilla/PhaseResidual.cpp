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
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   void Obs::dump(std::ostream& s) const
   {
      s << "phase:" << phase1
        << " dd:" << dd
        << " td:" << td;
   }

   std::ostream& operator << (std::ostream& s, const Obs& pr) 
   { pr.dump(s); return s; }


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   void Arc::computeTD(void)
   {
      iterator i = begin();
      while (i != end())
      {
         Obs& prev = i->second;
         i++;
         if (i == end())
            break;
         Obs& curr = i->second;
         curr.td = curr.dd - prev.dd;
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
   void Arc::dump(std::ostream& s) const
   {
      gpstk::Stats<double> stats = statsDD();

      const_iterator i=begin();
      const gpstk::DayTime& t0=i->first;
      i = end(); i--;
      const gpstk::DayTime& t1=i->first;

      s << std::left
        << "Arc: " << t0.printf("%02H:%02M:%04.1f")
        << " - "   << t1.printf("%02H:%02M:%04.1f")
        << "  N:" <<  std::setw(5) << stats.N();
      
      if (!garbage)
      {
         s << " master:" << std::setw(2) << master.id
           << " sdev:"  << std::setprecision(3) << std::setw(8)  << stats.StdDev()
           << " ddBias:"  << std::setprecision(12) << ddBias;
         if (stats.Average() > stats.StdDev()/sqrt((float)stats.N()))
            s << " avg:" << std::setprecision(4) << stats.Average();
         if (stats.StdDev() == 0)
            s << " Suspect!";
      }
      else
         s << " Garbage.";
      s << std::endl;

   }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
   std::ostream& operator <<(std::ostream& s, const Arc& arc)
   { arc.dump(s); return s; }


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
      for (iterator arc = begin(); arc != end(); arc++)
      {
         Arc::iterator i = adjacent_find(arc->begin(), arc->end(), timeGap(gapSize));
         if (i == arc->end())
            continue;

         // Make a new empty arc immedietly after the current arc and
         // move all the data from here to the end of the current arc into
         // the new arc.
         iterator nextArc = arc;
         nextArc++;
         nextArc = insert(nextArc, Arc());
         nextArc->master = arc->master;
         nextArc->ddBias = arc->ddBias;
         nextArc->insert(++i, arc->end());
         arc->erase(i, arc->end());
         arc = nextArc;
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
   void ArcList::splitOnTD(void)
   {
      for (iterator arc = begin(); arc != end(); arc++)
      {
         for (Arc::iterator i = arc->begin(); i != arc->end(); i++)
         {
            Obs& pr = i->second;
            bool jump = std::abs(pr.td) > 0.45;

            // If this followed by a equal and oppisite jump, it is an outlier,
            // not a real jump.
            if (jump)
            {
               Arc::iterator j = i; j++;
               if (j != arc->end() && std::abs(pr.td + j->second.td) < 0.45)
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

               nextArc->master = arc->master;
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
   void ArcList::mergeArcs(long arcLen, double arcTime, double gapTime)
   {
      // First mark arcs as garbage as appropriate
      for (iterator i = begin(); i != end(); i++)
      {
         double dt = (--(i->end()))->first - i->begin()->first;
         if (i->size() < arcLen || dt < arcTime)
            i->garbage = true;
      }

      iterator i = begin();
      while (i != end() )
      {
         Arc& prev = *i;
         i++;
         if (i == end())
            break;
         Arc& curr = *i;
         bool merge=false;

         if (std::abs(curr.ddBias - prev.ddBias) < 0.45 && 
             curr.master == prev.master && !prev.garbage && !curr.garbage)
            merge = true;
         else if (prev.garbage && curr.garbage &&
                  (curr.begin()->first - prev.rbegin()->first) < gapTime)
            merge = true;

         if (merge)
         {
            // First make the biases exactly the same
            curr.debiasDD(prev.ddBias - curr.ddBias);
            // Then put the data from the current arc into the previous
            prev.insert(curr.begin(), curr.end());
            // and kill the current arc
            i = erase(i);
            i--;
         }
      }
   }


//------------------------------------------------------------------------------
   void ArcList::dump(std::ostream& s) const
   {
      for (const_iterator i = begin(); i != end(); i++)
      {
         i->dump(s);
         const_iterator j = i;
         if (++j != end())
         {
            // Yes, this is the dark side of the STL
            double gap = j->begin()->first - i->rbegin()->first;
            if (gap > 3)
               s << "Gap: " << gap << " seconds" << std::endl;
         }
      }
         
      gpstk::Stats<double> stats;
      for (const_iterator i = begin(); i != end(); i++)
         if (!i->garbage)
            for (Arc::const_iterator j = (i->begin())++; j != i->end(); j++)
               stats.Add(j->second.dd);

      s << "ArcList N:" << stats.N()
        << " sdev:" << std::setprecision(4) << stats.StdDev();

      if (stats.Average() > stats.StdDev()/sqrt((float)stats.N()))
         s << " avg:" << std::setprecision(4) << stats.Average();

      s << std::endl;
   }

   std::ostream& operator <<(std::ostream& s, const ArcList& al) 
   { al.dump(s); return s; }

} // end of PhaseResidual namespace
