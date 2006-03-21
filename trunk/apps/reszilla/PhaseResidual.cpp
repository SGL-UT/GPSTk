#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/PhaseResidual.cpp#1 $"

#include "Stats.hpp"

#include "PhaseResidual.hpp"

namespace PhaseResidual
{
//------------------------------------------------------------------------------
   void Obs::dump(std::ostream& s) const
   {
      s << "phase:" << phase
        << " dd:" << dd
        << " td:" << td;
   }

   std::ostream& operator << (std::ostream& s, const Obs& pr) { pr.dump(s); }


//------------------------------------------------------------------------------
   void Arc::computeTD(void)
   {
      iterator i, j;
      for (i = j = begin(); i != end(); j=i++)
      {
         if (i == begin())
            continue;
         Obs& curr = i->second;
         Obs& prev = j->second;
         curr.td = curr.dd - prev.dd;
      }
   }

   void Arc::debiasDD(double bias)
   {
      ddBias = bias;
      for (iterator i = begin(); i != end(); i++)
         i->second.dd -= ddBias;
   }

   gpstk::Stats<double> Arc::statsDD(void) const
   {
      gpstk::Stats<double> stats;

      for (const_iterator i = begin(); i != end(); i++)
         stats.Add(i->second.dd);

      return stats;
   }

   void Arc::dump(std::ostream& s) const
   {
      gpstk::Stats<double> stats = statsDD();

      s << "Arc size:" << size()
        << " StdDev:" << stats.StdDev()
        << " Average:" << stats.Average()
        << std::endl;
   }

   std::ostream& operator <<(std::ostream& s, const Arc& arc) { arc.dump(s); }


//------------------------------------------------------------------------------
   void ArcList::split(void)
   {
      // First compute the triple differences for the first arc
      front().computeTD();

      // Now break the arc on big triple differences
      iterator arc = begin();
      for (Arc::iterator i = arc->begin(); i != arc->end(); i++)
      {
         Obs& pr = i->second;
         bool slip = std::abs(pr.td) > 0.45;
         if (slip)
         {
            int prevSize = arc->size();
            // move from here to end to the next arc
            Arc pra;
            push_back(pra);
            iterator nextArc = end();
            nextArc--;
            nextArc->insert(i, arc->end());
            arc->erase(i, arc->end());
            arc = nextArc;
            i = arc->begin();
         }
      }
   }

   void ArcList::debias(void)
   {
      for (iterator i = begin(); i != end(); i++)
         i->debiasDD(i->statsDD().Average());
   }

   void ArcList::dump(std::ostream& s) const
   {
      for (const_iterator i = begin(); i != end(); i++)
         i->dump(s);
         
      gpstk::Stats<double> stats;
      for (const_iterator i = begin(); i != end(); i++)
         for (Arc::const_iterator j = i->begin(); j != i->end(); j++)
            stats.Add(j->second.dd);

      s << "ArcList N:" << stats.N()
        << " StdDev:" << stats.StdDev()
        << " Average:" << stats.Average()
        << std::endl;
   }

   std::ostream& operator <<(std::ostream& s, const ArcList& al) { al.dump(s); }
} // end of PhaseResidual namespace
