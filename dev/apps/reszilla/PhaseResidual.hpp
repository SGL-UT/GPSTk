#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/PhaseResidual.hpp#5 $"

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
      double phase1;  // original phase in cycles
      double phase2;  // ditto for the other rx
      double dd;      // the double difference in cycles
      double td;      // the triple difference in cycles
      double snr;

      Obs():phase1(0), phase2(0),dd(0),td(0),snr(0){};

      void dump(std::ostream& s) const;
   };

   std::ostream& operator << (std::ostream& s, const Obs& pr);


   // This is a set of observations over time plus a bias that has been
   // removed from the double difference
   class Arc : public std::map<gpstk::DayTime, Obs>
   {
     public:
      Arc():ddBias(0), garbage(false){};
      void dump(std::ostream& s) const;

      double ddBias;
      gpstk::RinexPrn master;

      bool garbage;

      void computeTD();
      void debiasDD(double bias);

      gpstk::Stats<double> statsDD(void) const;

      bool operator<(const Arc& right) const
         { return (--end())->first < right.begin()->first;}
   };

   std::ostream& operator << (std::ostream& s, const Arc& arc);


   // This is a set of arcs. While this class doesn't enforce it, the will
   // be all from the same SV/observation type
   class ArcList : public std::list<Arc>
   {
     public:
      ArcList() {push_back(Arc());};

      // Split the arcs on data gaps
      void splitOnGaps(double gap);

      // Compute triple differences in the arcs,
      void computeTD();

      // Split arcs based upon jumps in the triple differences.
      void splitOnTD();

      // Search all arcs and find the first obs that matches the specified time.
      bool findObs(const gpstk::DayTime& t, Arc::const_iterator& obs);

      // Recombine adjcent arcs (that are seperated by less than maxGapTime
      // seconds) that have the same bias and master or are both shorter
      // than the specified arcLen/arcTime. The later will get marked
      // as garbage in the process.
      void mergeArcs(long arcLen, double arcTime, double maxGapTime);

      // make each arc have a zero mean
      void debiasDD();

      void dump(std::ostream& s) const;
   };

   std::ostream& operator << (std::ostream& s, const ArcList& pral);
}
#endif
