#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/PhaseResidual.hpp#1 $"

#ifndef PHASERESIDUAL_HPP
#define PHASERESIDUAL_HPP

#include <map>
#include <iostream>

#include "DayTime.hpp"
#include "RinexObsHeader.hpp"

#include "ElevationRange.hpp"

// This namespace is just to keep the identifiers meaninfull and short...
namespace PhaseResidual
{
   // The value of phase, double difference, triple difference for a single
   // sv observable at a point in time.
   class Obs
   {
   public:
      double phase, dd, td; // units of cycles

      Obs():phase(0),dd(0),td(0){};

      void dump(std::ostream& s) const;
   };

   std::ostream& operator << (std::ostream& s, const Obs& pr);


   // This is a set of observations over time plus a bias that has been
   // removed from the double difference
   class Arc : public std::map<gpstk::DayTime, Obs>
   {
     public:
      Arc():ddBias(0){};
      void dump(std::ostream& s) const;

      double ddBias;
      gpstk::RinexPrn master;

      void computeTD();
      void debiasDD(double bias);

      gpstk::Stats<double> statsDD(void) const;
   };

   std::ostream& operator << (std::ostream& s, const Arc& arc);


   // This is a set of arcs. While this class doesn't enforce it, the will
   // be all from the same SV/observation type
   class ArcList : public std::list<Arc>
   {
     public:
      ArcList() {push_back(Arc());};

      // compute triple differences in the first arc and then split that
      // arc up based upon jumps in the triple differences.
      void split();

      // make each arc have a zero mean
      void debias();

      void dump(std::ostream& s) const;
   };

   std::ostream& operator << (std::ostream& s, const ArcList& pral);
}
#endif
