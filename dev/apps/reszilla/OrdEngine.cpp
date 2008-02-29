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
   const XvtStore<SatID>& e,
   const WxObsData& w,
   const Triple& p,
   const string& mode,
   TropModel& t)
   : eph(e), wod(w), antennaPos(p), tm(t), mode(mode),
     oidSet(false),
     svTime(false), keepWarts(false), keepUnhealthy(false),
     wartCount(0), verboseLevel(0), debugLevel(0), dualFreq(false)
{
   if (RSS(antennaPos[0], antennaPos[1], antennaPos[2]) < 1)
   {
      cerr << "Warning! The antenna antennaPospears to be within one meter of the" << endl
           << "center of the geoid. This program is not cantennaPosable of" << endl
           << "accurately estimating the propigation of GNSS signals" << endl
           << "through solids such as a planetary crust or magma. Also," << endl
           << "if this location is correct, your antenna is probably" << endl
           << "no longer in the best of operating condition." << endl;
      exit(-1);
   }

   ECEF ecef(antennaPos);
   Geodetic geo(ecef, &gm);
   tm.setReceiverHeight(geo.getAltitude());
   tm.setReceiverLatitude(geo.getLatitude());
}

void OrdEngine::setMode(const ObsEpoch& obs)
{
   if (mode=="p1p2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcP);
      oid2 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcP);
      dualFreq = true;
   }
   if (mode=="z1z2" || mode=="w1w2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcW);
      oid2 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcW);
      dualFreq = true;
   }
   else if (mode=="c1p2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcCA);
      oid2 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcP);
      dualFreq = true;
   }
   else if (mode=="c1z2" || mode=="c1w2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcCA);
      oid2 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcW);
      dualFreq = true;
   }
   else if (mode=="y1y2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcY);
      oid2 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcY);
      dualFreq = true;
   }
   else if (mode=="c1y2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcCA);
      oid2 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcY);
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
   else if (mode=="y1")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcY);
   }
   else if (mode=="z1" || mode=="w1")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcW);
   }
   else if (mode=="c2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcC2LM);
   }
   else if (mode=="p2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcP);
   }
   else if (mode=="y2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcY);
   }
   else if (mode=="z2" || mode=="w2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcW);
   }
   else if (mode=="smo")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1L2,   ObsID::tcP);
      svTime = true;
   }
   else if (mode=="smart")
   {
      const SvObsEpoch& soe = obs.begin()->second;
      SvObsEpoch::const_iterator itr;

      for (itr = soe.begin(); itr != soe.end(); itr++)
      {
         const ObsID& oid = itr->first;
         if (oid.type != ObsID::otRange)
            continue;
         if (oid.band == ObsID::cbL1)
            oid1 = oid;
         if (oid.band == ObsID::cbL2)
         {
            oid2 = oid;
            dualFreq = true;
         }
         if (oid.band == ObsID::cbL1L2)
         {
            oid1 = oid;
            dualFreq = false;
            svTime = true;
         }
      }
   }
   else
   {
      cerr << "Unknown ORD computation requested, mode=" << mode << endl;
      exit(-1);
   }

   oidSet = true;

   if (verboseLevel)
   {
      cout << "# OrdEngine using " << mode << " mode" << endl;
      cout << "# OrdEngine using " << oid1;
      if (dualFreq)
         cout << " and " << oid2;
      cout << endl;
      if (svTime)
         cout << "# OrdEngine using SV time" << endl;
   }
}

// ---------------------------------------------------------------------
gpstk::ORDEpoch OrdEngine::operator()(const gpstk::ObsEpoch& obs)
{
   if (!oidSet)
      setMode(obs);

   const DayTime& t = obs.time;
   const ObsEpoch& obsEpoch = obs;

   ORDEpoch ordEpoch;
   ordEpoch.time = t;

   if (debugLevel>2)
      cout << " obs: " << obsEpoch.time << endl << obsEpoch;

   try
   {
      // Now set up our trop model for this epoch
      const WxObservation wx = wod.getMostRecent(t);
      tm.setDayOfYear(t.DOYday());
      if (wx.isAllValid())
      {
         if (debugLevel > 2)
            cout << " wx: " << wx << endl;
            
         // trop model will return errors for humidity over 100
         if (wx.humidity > 100)
            tm.setWeather(wx.temperature, wx.pressure, 100);
         else   
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

         // first we need to make sure the observation has the data we require.
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

         try
         {
            if (dualFreq)
               ordEpoch.ords[svid] = ObsRngDev(
                  obs1, obs2, svid, t, antennaPos, eph, gm, tm, svTime);
            else
               ordEpoch.ords[svid] = ObsRngDev(
                  obs1, svid, t, antennaPos, eph, gm, tm, svTime);

            ObsRngDev& ord = ordEpoch.ords[svid];

            // A gross check on the pseudorange
            const double rhoMin = 1e6; // Minimum reasonable pseudorange
            if (obs1 < rhoMin || (dualFreq && obs2 < rhoMin))
               ord.wonky |= 0x0001;

            // Any LLI indicator makes the data suspect
            for (k=svObsEpoch.begin(); k != svObsEpoch.end(); k++)
               if (k->first.type == ObsID::otLLI && k->second == 1)
                  ord.wonky |= 0x0002;

            // Make sure we have a valid C/A pseudorange unless we are a mixed
            // frequency 
            if (oid1.band != ObsID::cbL1L2)
            {
               const ObsID C1(ObsID::otRange,   ObsID::cbL1,   ObsID::tcCA);
               k = svObsEpoch.find(C1);
               if (k == svObsEpoch.end() || k->second < rhoMin)
                  ord.wonky |= 0x0004;
            }

            if (!keepUnhealthy && ord.getHealth().is_valid() && ord.getHealth())
               ord.wonky |= 0x0008;
         
            if (std::abs(ord.getTrop()) > 100)
               ord.wonky |= 0x0010;
         
            if (ord.getElevation() <= 0.05)
               ord.wonky |= 0x0020;

            if (debugLevel>2)
               cout << ord << endl;

         }
         catch (gpstk::Exception& e)
         {
            ordEpoch.ords.erase(svid);
            if (verboseLevel>2)
               cout << "#" << e << endl;
         }
      } // end looping over each SV in this epoch

   }
   catch (gpstk::Exception& e)
   {
      if (verboseLevel)
         cout << "#" << e;
   }

   return ordEpoch;
}
