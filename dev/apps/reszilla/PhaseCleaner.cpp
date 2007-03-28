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

#include "ObsID.hpp"

#include "PhaseCleaner.hpp"

using namespace std;
using namespace gpstk;


unsigned PhaseCleaner::debugLevel;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PhaseCleaner::PhaseCleaner(long al, double at, double gt)
   : minArcLen(al), minArcTime(at), maxGapTime(gt)
{
   lamda[ObsID::cbL1] = gpstk::C_GPS_M/gpstk::L1_FREQ;
   lamda[ObsID::cbL2] = gpstk::C_GPS_M/gpstk::L2_FREQ;
}


//-----------------------------------------------------------------------------
// Pulls the phase data data into arcs. Only data that exists on both receivers
// is included
//-----------------------------------------------------------------------------
void PhaseCleaner::addData(const gpstk::ObsEpochMap& rx1, const gpstk::ObsEpochMap& rx2)
{
   if (debugLevel)
      cout << "PhaseCleaner::addData(), " 
           << rx1.size() << ", " << rx2.size() << " epochs" << endl;

   // Now loop over all the epochs, pulling the data into the arcs
   for (gpstk::ObsEpochMap::const_iterator ei1=rx1.begin(); ei1!=rx1.end(); ei1++)
   {
      gpstk::DayTime t = ei1->first;
      const gpstk::ObsEpoch& rod1 = ei1->second;
      gpstk::ObsEpochMap::const_iterator ei2 = rx2.find(t);

      // Gotta have data from the other receiver
      if (ei2 == rx2.end())
         continue;
      const gpstk::ObsEpoch& rod2 = ei2->second;
      
      clockOffset[t] = rod1.rxClock - rod2.rxClock;

      for (gpstk::ObsEpoch::const_iterator pi1=rod1.begin(); pi1 != rod1.end(); pi1++)
      {
         const gpstk::SatID& prn = pi1->first;
         const gpstk::SvObsEpoch& rotm1 = pi1->second;

         // Make sure the other receiver saw this SV
         const gpstk::ObsEpoch::const_iterator pi2 = rod2.find(prn);
         if (pi2 == rod2.end())
            continue;
         const gpstk::SvObsEpoch& rotm2 = pi2->second;

         // We need a doppler, and any one will do
         gpstk::SvObsEpoch::const_iterator d;
         for (d = rotm1.begin(); d != rotm1.end(); d++)
            if (d->first.type == ObsID::otDoppler)
               break;

         // No doppler, no phase double difference. sorry
         if (d == rotm1.end())
            continue;

         double freq = d->first.band == ObsID::cbL2 ? gpstk::L2_FREQ : gpstk::L1_FREQ;
         rangeRate[prn][t] = d->second * gpstk::C_GPS_M/freq;

         gpstk::SvObsEpoch::const_iterator phase1;
         for (phase1 = rotm1.begin(); phase1 != rotm1.end(); phase1++)
         {
            const gpstk::ObsID& rot = phase1->first;
            if (rot.type != ObsID::otPhase)
               continue;

            gpstk::SvObsEpoch::const_iterator phase2 = rotm2.find(rot);
            if (phase2 == rotm2.end())
               continue;

            // Don't use the data if we have an SN in the data and it looks
            // bogus.
            double snr=-1;
            gpstk::ObsID srot = rot;
               srot.type = ObsID::otSNR;
            gpstk::SvObsEpoch::const_iterator snr1_itr = rotm1.find(srot);
            gpstk::SvObsEpoch::const_iterator snr2_itr = rotm2.find(srot);

            if (snr1_itr != rotm1.end() && snr2_itr != rotm2.end() )
            {
               snr = snr1_itr->second;
               if (std::abs(snr) < 1 || std::abs(snr2_itr->second) < 1)
                  continue;
            }

            // Note that we need the phase in cycles to make the PhaseResidual
            // class work right.
            PhaseResidual::Arc& arc = pot[rot][prn].front();
            arc[t].phase11 = phase1->second;
            arc[t].phase12 = phase2->second;
            arc[t].snr1 = snr;
         }
      }
   }
}


//-----------------------------------------------------------------------------
// This is intended to be a predicated for the for_each generic algorithm
// find the SV above indicated elevation with the lowest elevation, that is
// rising, and not equal to prn (the 'current' SV)
//-----------------------------------------------------------------------------
bool PhaseCleaner::goodMaster::operator()(const SvDoubleMap::value_type& pdm)
{
   bool good = (pdm.second > minVal)
      && (pdm.first != prn)
      && (rangeRate[pdm.first][time] >0);

   if (!good)
      return false;

   if (bestPrn.id < 1 || pdm.second < bestElev)
   {
      bestPrn = pdm.first;
      bestElev = pdm.second;
   }
   return true;
}


//-----------------------------------------------------------------------------
// This function can't be moved to PhaseResidual since it needs access to all
// the other PRN's data. Not that this should guarantee that the data exists
// from both the SVs.
//-----------------------------------------------------------------------------
void PhaseCleaner::selectMasters(
   const gpstk::ObsID& rot, 
   const gpstk::SatID& prn, 
   SvElevationMap& pem)
{
   PhaseResidual::ArcList& pral = pot[rot][prn];

   for (PhaseResidual::ArcList::iterator arc = pral.begin(); arc != pral.end(); arc++)
   {
      for (PhaseResidual::Arc::iterator i = arc->begin(); i != arc->end(); i++)
      {
         const gpstk::DayTime& t = i->first;
         PhaseResidual::Obs& obs = i->second;

         SvElevationMap::iterator j = pem.find(t);
         if (j == pem.end())
         {
            cout << "No elevation available. Returning." << t << endl;
            return;
         }
         SvDoubleMap& pdm = j->second;

         bool haveMasterObs = false;

         if (arc->master.id > 0)
         {
            PhaseResidual::Arc::const_iterator k;
            if (pot[rot][arc->master].findObs(t, k))
               haveMasterObs = true;
         }
         
         // See if we need a new master...
         if (!haveMasterObs || pdm[arc->master] < 10)
         {
            gpstk::SatID newMaster;
            goodMaster gm = for_each(pdm.begin(), pdm.end(),
                                     goodMaster(15, prn, t, rangeRate));
            if (gm.bestPrn.id > 0)
               newMaster = gm.bestPrn;
            else
            {
               if (debugLevel)
               {
                  cout << "Could not find a suitable master for prn " << prn.id
                       << " " << rot.type
                       << " at " << t
                       << endl;
                  SvDoubleMap::const_iterator e;
                  for (e = pdm.begin(); e != pdm.end(); e++)
                     cout << " prn: " << e->first.id
                          << ", elev:" << e->second
                          << ", rate:" << rangeRate[e->first][t]
                          << endl;
               }
                  return;
            }

            PhaseResidual::Arc::const_iterator k;
            if (!pot[rot][newMaster].findObs(t, k))
            {
               if (debugLevel)
                  cout << t << " rot:" << rot << " newMaster:" << newMaster 
                       << " Selected an invalid master." << endl;
               return;
            }
               
            if (debugLevel)
               cout << t << " prn " << newMaster.id << " as master. (" << rot 
                    << ")" << endl;
            
            if (arc->master.id < 1)
            {
               arc->master = newMaster;
            }
            else
            {
               PhaseResidual::ArcList::iterator nextArc = arc; nextArc++;
               nextArc = pral.insert(nextArc, PhaseResidual::Arc());
               nextArc->master = newMaster;
               nextArc->insert(i, arc->end());
               arc->erase(i, arc->end());
               break;
            }
         }
      }
   }
} // PhaseCleaner::splitMaster()


//-----------------------------------------------------------------------------
// This function can't be moved to PhaseResidual since it needs access to all
// the other PRN's data. Note that this function assumes that there is data
// for the master SV.
//-----------------------------------------------------------------------------
void PhaseCleaner::doubleDifference(
   const gpstk::ObsID& rot, 
   const gpstk::SatID& prn,
   SvElevationMap& pem)
{
   PhaseResidual::ArcList& pral = pot[rot][prn];

   for (PhaseResidual::ArcList::iterator arc = pral.begin(); arc != pral.end(); arc++)
   {
      if (arc->master.id <1)
         continue;

      for (PhaseResidual::Arc::iterator i = arc->begin(); i != arc->end(); i++)
      {
         const gpstk::DayTime& t = i->first;
         PhaseResidual::Obs& obs = i->second;

         PhaseResidual::Arc::const_iterator k;
         if (!pot[rot][arc->master].findObs(t, k))
            continue;
         
         const PhaseResidual::Obs& masterObs = k->second;
         
         // Now compute the dd for this epoch
         double masterDiff = masterObs.phase11 - masterObs.phase12;
         double coc = (clockOffset[t]) * (rangeRate[arc->master][t]) / lamda[rot.band];
         masterDiff -= coc;

         double myDiff = obs.phase11 - obs.phase12;
         coc = clockOffset[t] * rangeRate[prn][t] / lamda[rot.band];
         myDiff -= coc;

         obs.dd = masterDiff - myDiff;
      }
   }
} // PhaseCleaner::doubleDifference()


//-----------------------------------------------------------------------------
// This is one call to do all the work. All the other functions should be
// call by this one.
//-----------------------------------------------------------------------------
void PhaseCleaner::debias(SvElevationMap& pem)
{
   if (debugLevel)
      cout << "PhaseCleaner::debias()" << endl;

   // At this point, the pot has only phase1 & phase2 set.
   // Also only one arc exists for each prn; and that arc doesn't even
   // have the master prn set.
   for (PraPrnOt::iterator i = pot.begin(); i != pot.end(); i++)
   {
      const gpstk::ObsID& rot = i->first;
      PraPrn& praPrn = i->second;
      for (PraPrn::iterator j = praPrn.begin(); j != praPrn.end(); j++)
      {
         const gpstk::SatID& prn = j->first;
         PhaseResidual::ArcList& pral = j->second;

         pral.splitOnGaps(maxGapTime);
         selectMasters(rot, prn, pem);
         doubleDifference(rot, prn, pem);

         pral.computeTD();
         pral.splitOnTD();
         pral.debiasDD();

         pral.computeTD();
         pral.splitOnTD();
         pral.debiasDD();

         pral.mergeArcs(minArcLen, minArcTime, maxGapTime);

         if (debugLevel>1)
            cout << "Done cleaning " << prn.id << " on " << rot.type << endl
                 << pral;
      }
   }   
}  // end of debias()


//-----------------------------------------------------------------------------
// Gets the double differences and puts them back into the DDEpochMap
//-----------------------------------------------------------------------------
void PhaseCleaner::getPhaseDD(DDEpochMap& ddem) const
{
   if (debugLevel)
      cout << "putting phases back into ddem" << endl;

   // Really should use pot to walk through the data...
   for (PraPrnOt::const_iterator i = pot.begin(); i != pot.end(); i++)
   {
      const gpstk::ObsID& rot = i->first;
      const PraPrn& pp = i->second;
 
      for (PraPrn::const_iterator j = pp.begin(); j != pp.end(); j++)
      {
         const gpstk::SatID& prn = j->first;
         const PhaseResidual::ArcList& al = j->second;

         for (PhaseResidual::ArcList::const_iterator k = al.begin(); k != al.end(); k++)
         {
            const PhaseResidual::Arc& arc = *k;

            for (PhaseResidual::Arc::const_iterator l = arc.begin(); l != arc.end(); l++)
            {
               const gpstk::DayTime& t = l->first;
               const PhaseResidual::Obs& obs = l->second;

               // Whew! thats deep. Now to stuff the dd back in to the ddem
               // remember that ddem has it's values in meters
               if (arc.garbage)
                  ddem[t].dd[prn][rot] = 0;
               else
                  ddem[t].dd[prn][rot] = obs.dd * lamda[rot.band];
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
// Make a list of "real" cycle slips
//-----------------------------------------------------------------------------
void PhaseCleaner::getSlips(
   CycleSlipList& csl, 
   SvElevationMap& pem) const
{
   for (PraPrnOt::const_iterator i = pot.begin(); i != pot.end(); i++)
   {
      const gpstk::ObsID& rot = i->first;
      const PraPrn& praPrn = i->second;

      for (PraPrn::const_iterator j = praPrn.begin(); j != praPrn.end(); j++)
      {
         const gpstk::SatID& prn = j->first;
         const PhaseResidual::ArcList& al = j->second;

         PhaseResidual::ArcList::const_iterator k = al.begin();
         while (k != al.end())
         {
            const PhaseResidual::Arc& arc0 = *k;
            k++;
            if (k == al.end())
               break;

            const PhaseResidual::Arc& arc1 = *k;

            if (arc0.garbage || arc1.garbage ||
                arc0.master != arc1.master)
               continue;

            const gpstk::DayTime& t1Begin = arc1.begin()->first;
            PhaseResidual::Arc::const_iterator l = arc0.end(); l--;
            const gpstk::DayTime& t0End = l->first;
            
            if (std::abs(t1Begin-t0End) > maxGapTime)
               continue;

            l = arc1.end(); l--;
            const gpstk::DayTime& t1End = l->first;
            const gpstk::DayTime& t0Begin = arc0.begin()->first;
            
            double t0Len = t0End - t0Begin;
            double t1Len = t1End - t1Begin;
            if (t0Len < minArcTime || t1Len < minArcTime)
               continue;

            CycleSlipRecord cs;
            cs.t = t1Begin;
            cs.cycles = (arc1.ddBias - arc0.ddBias);
            cs.oid = rot;
            cs.prn = prn;
            cs.elevation = pem[t1Begin][prn];
            cs.masterPrn = arc1.master;
            cs.postCount = arc1.size();
            cs.preCount = arc0.size();
            cs.preGap = t1Begin - t0End;
            csl.push_back(cs);
         }
      }  
   }

   // Now to sort these on time.
   csl.sort();
}


//-----------------------------------------------------------------------------
// Dump the maps to the standard table format...
//-----------------------------------------------------------------------------
void PhaseCleaner::dump(std::ostream& s) const
{
   s << "# time              PRN type  elev      clk(m)"
     << "    2nd diff(m)"
     << endl;

   for (PraPrnOt::const_iterator i = pot.begin(); i != pot.end(); i++)
   {
      const gpstk::ObsID& rot = i->first;
      const PraPrn& pp = i->second;
 
      for (PraPrn::const_iterator j = pp.begin(); j != pp.end(); j++)
      {
         const gpstk::SatID& prn = j->first;
         const PhaseResidual::ArcList& al = j->second;

         for (PhaseResidual::ArcList::const_iterator k = al.begin(); k != al.end(); k++)
         {
            const PhaseResidual::Arc& arc = *k;

            for (PhaseResidual::Arc::const_iterator l = arc.begin(); l != arc.end(); l++)
            {
               const gpstk::DayTime& t = l->first;
               const PhaseResidual::Obs& obs = l->second;

               s.setf(ios::fixed, ios::floatfield);
               s << left << setw(20) << t << right
                 << setfill(' ')
                 << " " << setw(2) << prn.id
                 << " " << ObsID::cbStrings[rot.band]
                 << " " << setprecision(1) << setw(5)  << 0  // elevation
                 << " " << setprecision(3) << setw(12)  << 0 // clock
                 << " " << setprecision(6) << setw(14) << obs.dd * lamda[rot.band]
                 << endl;
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

unsigned PhaseCleanerA::debugLevel;

//-----------------------------------------------------------------------------
// Pulls the phase data data into arcs. Only data that exists on both receivers
// is included
//-----------------------------------------------------------------------------
void PhaseCleanerA::addData(const ObsEpochMap& rx1, const ObsEpochMap& rx2)
{
   if (debugLevel)
      cout << "PhaseCleanerA::addData(), " 
           << rx1.size() << ", " << rx2.size() << " epochs" << endl;


   // Now loop over all the epochs, pulling the data into the arcs
   for (ObsEpochMap::const_iterator ei1=rx1.begin(); ei1!=rx1.end(); ei1++)
   {
      DayTime t = ei1->first;
      const ObsEpoch& oe1 = ei1->second;
      ObsEpochMap::const_iterator ei2 = rx2.find(t);

      // Gotta have data from the other receiver
      if (ei2 == rx2.end())
         continue;
      const ObsEpoch& oe2 = ei2->second;
      
      double clockOffset = oe1.rxClock - oe2.rxClock;

      // SV line-of-sight motion, in meters/second
      map<gpstk::SatID, double> rangeRate;

      // First we need to get a range rates for all SVs
      for (ObsEpoch::const_iterator i=oe1.begin(); i != oe1.end(); i++)
      {
         const SatID& sv = i->first;
         const SvObsEpoch& soe = i->second;

         // We need a doppler, and any one will do
         SvObsEpoch::const_iterator d;
         for (d = soe.begin(); d != soe.end(); d++)
            if (d->first.type == ObsID::otDoppler)
               break;

         if (d == soe.end())
            continue;

         double freq = d->first.band == ObsID::cbL2 ? L2_FREQ : L1_FREQ;
         rangeRate[sv] = d->second * C_GPS_M/freq;
      }

      // Loop over all SVs in track on reciever #1
      for (ObsEpoch::const_iterator pi11=oe1.begin(); pi11 != oe1.end(); pi11++)
      {
         const SatID& sv1 = pi11->first;
         const SvObsEpoch& soe11 = pi11->second; // SV #1, Rx #1

         // Make sure receiver #2 saw SV #1
         const ObsEpoch::const_iterator pi12 = oe2.find(sv1);
         if (pi12 == oe2.end())
            continue;
         const SvObsEpoch& soe12 = pi12->second; // SV #1, Rx #2

         // Here we loop over all the 'other' SVs in track on receiver #1
         for (ObsEpoch::const_iterator pi21=pi11; pi21 != oe1.end(); pi21++)
         {
            if (pi21 == pi11)
               continue;

            const SatID& sv2 = pi21->first;
            const SvObsEpoch& soe21 = pi21->second; // SV #2, Rx #1

            // Make sure receiver #2 saw SV #2 
            ObsEpoch::const_iterator pi22 = oe2.find(sv2);
            if (pi22 == oe2.end())
               continue;
            const SvObsEpoch& soe22 = pi22->second;  // SV #2, Rx #2

            SatIdPair svPair(sv1, sv2);

            if (debugLevel>2)
               cout << t << "  " << sv1 << "-" << sv2 << endl;

            // Now go throgh all phase observations from SV #1, Rx #1
            SvObsEpoch::const_iterator phase11;
            for (phase11 = soe11.begin(); phase11 != soe11.end(); phase11++)
            {
               const ObsID& rot = phase11->first;
               if (rot.type != ObsID::otPhase)
                  continue;

               // Make sure that we have phase data from SV #1, Rx #2
               SvObsEpoch::const_iterator phase12 = soe12.find(rot);
               if (phase12 == soe12.end())
                  continue;

               // Make sure that we have phase data from SV #2, Rx #1
               SvObsEpoch::const_iterator phase21 = soe21.find(rot);
               if (phase21 == soe21.end())
                  continue;

               // Make sure that we have phase data from SV #2, Rx #2
               SvObsEpoch::const_iterator phase22 = soe22.find(rot);
               if (phase22 == soe22.end())
                  continue;

               // Don't use the data if we have an SNR in the data and it looks
               // bogus.
               double snr=-1;
               ObsID srot = rot;
               srot.type = ObsID::otSNR;
               SvObsEpoch::const_iterator snr_itr = soe11.find(srot);

               if (snr_itr != soe11.end())
               {
                  snr = snr_itr->second;
                  if (std::abs(snr) < 1 )
                     continue;
               }

               // And we can't compute our clock correction without the
               // doppler
               if (rangeRate[sv1] == 0 || rangeRate[sv2] ==0)
                  continue;
         
               // Note that we need the phase in cycles to make the PhaseResidual
               // class work right.
               PhaseResidual::Arc& arc = pot[rot][svPair].front();
               PhaseResidual::Obs& obs = arc[t];
               obs.phase11 = phase11->second;
               obs.phase12 = phase12->second;
               obs.phase21 = phase21->second;
               obs.phase22 = phase22->second;
               obs.snr1 = snr;

               double lamdaInv;
               if (rot.band == ObsID::cbL1)
                  lamdaInv = L1_FREQ/C_GPS_M;
               else if (rot.band == ObsID::cbL2)
                  lamdaInv = L2_FREQ/C_GPS_M;
               else
                  continue;

               // Now compute the dd for this epoch
               double sd1 = obs.phase11 - obs.phase12;
               double coc = clockOffset * rangeRate[sv1] * lamdaInv;
               sd1 -= coc;
               
               double sd2 = obs.phase21 - obs.phase22;
               coc = clockOffset * rangeRate[sv2] * lamdaInv;
               sd2 -= coc;

               obs.dd = sd1 - sd2;
            }
         }
      }
   }
}



//-----------------------------------------------------------------------------
// This is one call to do all the work. All the other functions should be
// call by this one.
//-----------------------------------------------------------------------------
void PhaseCleanerA::debias(SvElevationMap& pem)
{
   if (debugLevel)
      cout << "PhaseCleanerA::debias()" << endl;

   // At this point, the pot has all phases set and the double difference
   // computed. Only one arc exists for each prn pair.
   for (PraSvPrOt::iterator i = pot.begin(); i != pot.end(); i++)
   {
      const ObsID& rot = i->first;
      PraSvPair& praSv = i->second;
      for (PraSvPair::iterator j = praSv.begin(); j != praSv.end(); j++)
      {
         const SatIdPair& svPair = j->first;
         PhaseResidual::ArcList& pral = j->second;

         pral.splitOnGaps(maxGapTime);

         pral.computeTD();
         pral.splitOnTD();
         pral.debiasDD();

         pral.mergeArcs(minArcLen, minArcTime, maxGapTime);

         if (debugLevel>1)
            cout << "Done cleaning " << svPair.first
                 << ":" << svPair.second
                 << " on " << rot.type << endl
                 << pral;
      }
   }   
}  // end of debias()


//-----------------------------------------------------------------------------
// Gets the double differences and puts them back into the DDEpochMap
//-----------------------------------------------------------------------------
void PhaseCleanerA::getPhaseDD(DDAEpochMap& ddem) const
{
   if (debugLevel)
      cout << "putting phases back into ddem" << endl;

   // Really should use pot to walk through the data...
   for (PraSvPrOt::const_iterator i = pot.begin(); i != pot.end(); i++)
   {
      const ObsID& rot = i->first;
      const PraSvPair& pp = i->second;

      double lamda;
      if (rot.band == ObsID::cbL1)
         lamda = C_GPS_M/L1_FREQ;
      else if (rot.band == ObsID::cbL2)
         lamda = C_GPS_M/L2_FREQ;
      else
         continue;

      for (PraSvPair::const_iterator j = pp.begin(); j != pp.end(); j++)
      {
         const SatIdPair& svPair = j->first;
         const PhaseResidual::ArcList& al = j->second;

         for (PhaseResidual::ArcList::const_iterator k = al.begin(); k != al.end(); k++)
         {
            const PhaseResidual::Arc& arc = *k;

            for (PhaseResidual::Arc::const_iterator l = arc.begin(); l != arc.end(); l++)
            {
               const DayTime& t = l->first;
               const PhaseResidual::Obs& obs = l->second;

               // Whew! thats deep. Now to stuff the dd back in to the ddem
               // remember that ddem has it's values in meters
               if (arc.garbage)
                  ddem[t].dd[svPair][rot] = 0;
               else
                  ddem[t].dd[svPair][rot] = obs.dd * lamda;
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
// Dump the maps to the standard table format...
//-----------------------------------------------------------------------------
void PhaseCleanerA::dump(std::ostream& s) const
{
   s << "# time              PRN type  elev      clk(m)"
     << "    2nd diff(m)"
     << endl;

   for (PraSvPrOt::const_iterator i = pot.begin(); i != pot.end(); i++)
   {
      const ObsID& rot = i->first;
      const PraSvPair& pp = i->second;
 
      double lamda;
      if (rot.band == ObsID::cbL1)
         lamda = C_GPS_M/L1_FREQ;
      else if (rot.band == ObsID::cbL2)
         lamda = C_GPS_M/L2_FREQ;
      else
         continue;

      for (PraSvPair::const_iterator j = pp.begin(); j != pp.end(); j++)
      {
         const SatIdPair& svPair = j->first;
         const PhaseResidual::ArcList& al = j->second;

         for (PhaseResidual::ArcList::const_iterator k = al.begin(); k != al.end(); k++)
         {
            const PhaseResidual::Arc& arc = *k;

            for (PhaseResidual::Arc::const_iterator l = arc.begin(); l != arc.end(); l++)
            {
               const DayTime& t = l->first;
               const PhaseResidual::Obs& obs = l->second;

               s.setf(ios::fixed, ios::floatfield);
               s << left << setw(20) << t << right
                 << setfill(' ')
                 << " " << setw(2) << svPair.first.id
                 << " " << setw(2) << svPair.second.id
                 << " " << ObsID::cbStrings[rot.band]
                 << " " << setprecision(1) << setw(5)  << 0  // elevation
                 << " " << setprecision(3) << setw(12)  << 0 // clock
                 << " " << setprecision(6) << setw(14) << obs.dd * lamda
                 << endl;
            }
         }
      }
   }
}
