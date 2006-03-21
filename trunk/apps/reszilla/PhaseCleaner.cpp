#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/PhaseCleaner.cpp#1 $"

#include "PhaseCleaner.hpp"

using namespace std;


PhaseCleaner::PhaseCleaner()
{
   lamda[L1] = gpstk::C_GPS_M/gpstk::L1_FREQ;
   lamda[L2] = gpstk::C_GPS_M/gpstk::L2_FREQ;

   phaseObsTypes.insert(L1);
   phaseObsTypes.insert(L2);

   if (verbosity)
      cout << "1 mm on L1 is " << 0.001/lamda[L1] << " cycles" << endl
           << "1 mm on L2 is " << 0.001/lamda[L2] << " cycles" << endl;
}


//-----------------------------------------------------------------------------
// Pulls the phase data data from the double difference epochs into arcs.
//-----------------------------------------------------------------------------
void PhaseCleaner::addData(
   const RODEpochMap& rx1, 
   const RODEpochMap& rx2, 
   const DDEpochMap& ddem)
{
   if (verbosity)
      cout << "Pulling phase data into arcs." << endl;
   
   // Now loop over all the epochs, pulling the data into the arcs
   for (RODEpochMap::const_iterator ei1=rx1.begin(); ei1!=rx1.end(); ei1++)
   {
      gpstk::DayTime t = ei1->first;
      const gpstk::RinexObsData& rod1 = ei1->second;
      RODEpochMap::const_iterator ei2 = rx2.find(t);
      
      for (RinexPrnMap::const_iterator pi1=rod1.obs.begin(); pi1 != rod1.obs.end(); pi1++)
      {
         const gpstk::RinexPrn& prn = pi1->first;
         const gpstk::RinexObsData::RinexObsTypeMap& rotm = pi1->second;

         DDEpochMap::const_iterator ddi = ddem.find(t);
         if (ddi == ddem.end())
            continue;

         PrnROTDM::const_iterator proti = ddi->second.dd.find(prn);
         if (proti == ddi->second.dd.end())
            continue;

         RinexObsTypeSet::const_iterator rot_itr;
         for (rot_itr = phaseObsTypes.begin(); rot_itr != phaseObsTypes.end(); rot_itr++)
         {
            const RinexObsType& rot = *rot_itr;
            ROTDM::const_iterator rotdi = proti->second.find(rot);
            RinexObsTypeMap::const_iterator l1 = rotm.find(rot);
            if (rotdi == proti->second.end() || l1 == rotm.end())
               continue;

            // Initially add all data to the first arc for this prn -
            // it gets split up later.
            PhaseResidual::Arc& arc = pot[rot][prn].front();
            // get the double differences
            arc[t].dd = rotdi->second/lamda[rot];
            arc.master = ddi->second.masterPrn;

            // Get the phases from the original obs
            arc[t].phase = l1->second.data/lamda[rot];
         }
      }
   }
}


void PhaseCleaner::debias(void)
{
   if (verbosity)
      cout << "Debiasing phase data." << endl;

   // At this point, the pot has the phase, dd, and master prn set.
   for (PraPrnOt::iterator i = pot.begin(); i != pot.end(); i++)
   {
      const RinexObsType& rot = i->first;
      PraPrn& praPrn = i->second;

      for (PraPrn::iterator j = praPrn.begin(); j != praPrn.end(); j++)
      {
         const gpstk::RinexPrn& prn = j->first;
         PhaseResidual::ArcList& pral = j->second;
         if (verbosity>1)
            cout << "Debiasing prn " << prn.prn << " " << rot.type << endl;
         pral.split();
         pral.debias();
         if (verbosity>1)
            cout << pral;
      }
   }
}  // end of debias()

//-----------------------------------------------------------------------------
// Gets the double differences and puts them back into the DDEpochMap
//-----------------------------------------------------------------------------
void PhaseCleaner::getPhaseDD(DDEpochMap& ddem) const
{
   if (verbosity)
      cout << "Putting phase data back into the ddem." << endl;
   
   // Really should use pot to walk through the data...

   for (PraPrnOt::const_iterator i = pot.begin(); i != pot.end(); i++)
   {
      const gpstk::RinexObsHeader::RinexObsType& rot = i->first;
      const PraPrn& pp = i->second;
 
      for (PraPrn::const_iterator j = pp.begin(); j != pp.end(); j++)
      {
         const gpstk::RinexPrn& prn = j->first;
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
               ddem[t].dd[prn][rot] = obs.dd * lamda[rot];
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
// Gets the double differences and puts them back into the DDEpochMap
//-----------------------------------------------------------------------------
void PhaseCleaner::getSlips(CycleSlipList& csl) const
{
   // At this point, the pot has the phase, dd, and master prn set.
   for (PraPrnOt::const_iterator i = pot.begin(); i != pot.end(); i++)
   {
      const RinexObsType& rot = i->first;
      const PraPrn& praPrn = i->second;

      for (PraPrn::const_iterator j = praPrn.begin(); j != praPrn.end(); j++)
      {
         const gpstk::RinexPrn& prn = j->first;
         const PhaseResidual::ArcList& al = j->second;

         for (PhaseResidual::ArcList::const_iterator k = al.begin(); k != al.end(); k++)
         {
            if (k==al.begin())
               continue;
            const PhaseResidual::Arc& arc = *k;
            if (arc.size() > 5)
            {
               PhaseResidual::ArcList::const_iterator l = k;
               l--;
               const PhaseResidual::Arc& prev_arc = *l;
               
               CycleSlipRecord cs;
               cs.t = arc.begin()->first;
               cs.cycles = (arc.ddBias - prev_arc.ddBias);
               cs.rot = rot;
               cs.prn = prn;
               cs.masterPrn = arc.master;
               cs.postCount = arc.size();
               cs.preCount = prev_arc.size();
            }
         }
      }
  
   }
}
