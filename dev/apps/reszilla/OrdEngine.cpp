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

#include "OrdEngine.hpp"

using namespace std;
using namespace gpstk;


// ---------------------------------------------------------------------
OrdEngine::OrdEngine(
   const gpstk::EphemerisStore& e,
   const gpstk::WxObsData& w,
   const gpstk::Triple& p,
   gpstk::TropModel& t)
   : eph(e), wod(w), antennaPos(p), tm(t),
     svTime(false), keepWarts(false), keepUnhealthy(false),
     wartCount(0), verbosity(0), dualFreq(false)
{

   if (RSS(antennaPos[0], antennaPos[1], antennaPos[2]) < 1)
   {
      cerr << "Warning! The antenna antennaPospears to be within one meter of the" << endl
           << "center of the geoid. This program is not cantennaPosable of" << endl
           << "accurately estimating the propigation of GNSS signals" << endl
           << "through solids such as a planetary crust or magma. Also," << endl
           << "if this location is correct, your antenna is probally" << endl
           << "no longer in the best of operating condition." << endl;
      exit(-1);
   }

   ECEF ecef(antennaPos);
   Geodetic geo(ecef, &gm);
   tm.setReceiverHeight(geo.getAltitude());
   tm.setReceiverLatitude(geo.getLatitude());
}

void OrdEngine::setMode(const string& mode)
{
   if (mode=="p1p2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcP);
      oid2 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcP);
      dualFreq = true;
   }
   else if (mode=="c1p2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcCA);
      oid2 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcP);
      dualFreq = true;
   }
   else if (mode=="c1")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcCA);
   }
   else if (mode=="p1")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcP);
   }
   else if (mode=="c2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcC2LM);
   }
   else if (mode=="p2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcP);
   }
   else if (mode=="smo")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1L2,   ObsID::tcP);
   }
   else
   {
      cerr << "Unknown ORD computation requested, mode=" << mode << endl;
      exit(-1);
   }

   if (verbosity>1)
   {
      cout << "# Using " << oid1;
      if (dualFreq)
         cout << " and " << oid2;
      cout << endl;
   }
}

// ---------------------------------------------------------------------
gpstk::ORDEpoch OrdEngine::operator()(const gpstk::ObsEpoch& obs)
{
   const DayTime& t = obs.time;
   const ObsEpoch& obsEpoch = obs;

   ORDEpoch ordEpoch;
   ordEpoch.time = t;

   if (verbosity>3)
      cout << "#obs: " << obsEpoch.time << endl << obsEpoch;

   try
   {
      // Now set up our trop model for this epoch
      const WxObservation wx = wod.getMostRecent(t);
      tm.setDayOfYear(t.DOYday());
      if (wx.isAllValid())
      {
         if (verbosity>3)
            cout << "#wx: " << wx << endl;
         tm.setWeather(wx.temperature, wx.pressure, wx.humidity);
      }

      // Walk over all prns in this epoch
      ObsEpoch::const_iterator j;
      for (j=obsEpoch.begin(); j != obsEpoch.end(); j++)
      {
         const SatID svid = j->first;
         const SvObsEpoch& svObsEpoch = j->second;

         SvObsEpoch::const_iterator k;
         double obs1, obs2;

         // first we need to make sure the observation data (in rotm) has
         // the data we require.
         k = svObsEpoch.find(oid1);
         if (k == svObsEpoch.end())
            continue;
         else
            obs1 = k->second;
            
         if (dualFreq)
         {
            k = svObsEpoch.find(oid2);
            if (k == svObsEpoch.end())
               continue;
            else
               obs2 = k->second;
         }

         // Now to look for indications that this data is suspect
         if (!keepWarts)
         {
            // A gross check on the pseudorange
            if (obs1 < 15e6)
               continue;

            // If there is a LLI on any of the data, ignore the whole obs
            bool wonky=false;
            for (k=svObsEpoch.begin(); !wonky && k != svObsEpoch.end(); k++)
               if (k->first.type == ObsID::otLLI)
                  wonky=true;
            if (wonky)
               continue;

            if (dualFreq)
            {
               if (obs2 < 15e6)
                  continue;

               // Now make sure we have a valid C/A pseudorange
               const ObsID C1(ObsID::otRange,   ObsID::cbL1,   ObsID::tcCA);
               k = svObsEpoch.find(C1);
               if (k == svObsEpoch.end() || k->second < 15e6)
                  continue;
            }
         }

         try
         {
            if (dualFreq)
               ordEpoch.ords[svid] = ObsRngDev(
                  obs1, obs2, svid, t, antennaPos, eph, gm, tm, svTime);
            else
               ordEpoch.ords[svid] = ObsRngDev(
                  obs1, svid, t, antennaPos, eph, gm, tm, svTime);
         }
         catch (EphemerisStore::NoEphemerisFound& e)
         {
            if (verbosity>2)
               cout << e << endl;
         }
      } // end looping over each SV in this epoch

      if (!keepWarts)
      {
         ORDEpoch::ORDMap::iterator pi;
         for (pi = ordEpoch.ords.begin(); pi != ordEpoch.ords.end();)
         {
            const ObsRngDev& ord = pi->second;
            ORDEpoch::ORDMap::iterator pi2=pi;
            pi++;

            if (!keepUnhealthy && ord.getHealth().is_valid() && ord.getHealth())
            {
               ordEpoch.ords.erase(pi2);
               if (verbosity>3)
                  cout << "# Tossing ord from an unhealty SV." << endl;
               continue;
            }
               
            if (std::abs(ord.getTrop()) > 100 || ord.getElevation() <= 0.05)
            {
               ordEpoch.ords.erase(pi2);
               if (verbosity>1)
                  cout << "# Tossing wonky ord: " << ord << endl;
               continue;
            }
         }
      } // end deleting bad stuff


   }
   catch (gpstk::InvalidParameter& e)
   {
      if (verbosity>2)
         cout << e;
   }
   catch (gpstk::Exception& e)
   {
      if (verbosity)
         cout << e;
   }

   return ordEpoch;
}
