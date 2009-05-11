#pragma ident "$Id$"

/*
  This intended to perform a quick summary/analysis of the data in a MDP file
  or stream. The idea is teqc +meta or +mds with a little bit of +qc thrown
  in for good measure
*/

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
//============================================================================

#include "Geodetic.hpp"
#include "TrackProc.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


//-----------------------------------------------------------------------------
MDPTrackProcessor::MDPTrackProcessor(gpstk::MDPStream& in, std::ofstream& out)
   : MDPProcessor(in, out),
   currCv(13), prevCv(13)
{
   timeFormat = "%02H:%02M:%04.1f";
   for (int i=1; i<currCv.size(); i++)
      currCv[i].prn=-1;
   for (int i=1; i<prevCv.size(); i++)
      prevCv[i].prn=-1;

   obsOut = true;
}


MDPTrackProcessor::~MDPTrackProcessor()
{}

void MDPTrackProcessor::process(const gpstk::MDPObsEpoch& oe)
{
   if (oe.time != currTime)
   {
      printChanges();
      prevTime = currTime;
      currTime = oe.time;
      prevCv = currCv;
      for (int i=1; i<currCv.size(); i++)
         currCv[i].prn=-1;
   }

   int prn=oe.prn;
   int chan=oe.channel;

   if (chan<1 || chan >12)
      cout << "This program can only handles channles 1...12" << endl, exit(-1);

   // make a set of the obs that this epoch has
   rc_set ccs;
   currCv[chan].codes = "    ";
   for (gpstk::MDPObsEpoch::ObsMap::const_iterator i = oe.obs.begin();
        i != oe.obs.end(); i++)
   {
      const gpstk::MDPObsEpoch::Observation& obs=i->second;
      rcpair rcPair(obs.range, obs.carrier);
      ccs.insert(rcPair);
      if (obs.carrier == ccL1)
      {
         if      (obs.range == rcCA)       currCv[chan].codes[0] = 'c';
         if      (obs.range == rcPcode)    currCv[chan].codes[1] = 'p';
         else if (obs.range == rcYcode)    currCv[chan].codes[1] = 'y';
         else if (obs.range == rcCodeless) currCv[chan].codes[1] = 'z';
      }
      else if (obs.carrier == ccL2)
      {
         if      (obs.range == rcCM)       currCv[chan].codes[2] = 'm';
         else if (obs.range == rcCL)       currCv[chan].codes[2] = 'l';
         else if (obs.range == rcCMCL)     currCv[chan].codes[2] = 'x';
         else if (obs.range == rcCA)       currCv[chan].codes[2] = 'c';
         if      (obs.range == rcPcode)    currCv[chan].codes[3] = 'p';
         else if (obs.range == rcYcode)    currCv[chan].codes[3] = 'y';
         else if (obs.range == rcCodeless) currCv[chan].codes[3] = 'z';
      }
   }
   currCv[chan].obs = ccs;
   currCv[chan].prn = oe.prn;
   currCv[chan].elevation = oe.elevation;
}

void MDPTrackProcessor::printChanges()
{
   if (verboseLevel)
   {
      // This is the one line per channel format.
      for (int i = 1; i < currCv.size(); i++)
      {
         // This means that there has been a change in track
         bool change = currCv[i].obs != prevCv[i].obs ||
            currCv[i].prn != prevCv[i].prn ||
            (prevCv[i].prn == -1 && currCv[i].prn == -1);
         if (change)
         {
            if (prevCv[i].prn == -1 && currCv[i].prn == -1)
               continue;
            out << currTime.printf(timeFormat) << "  Ch:" << setw(2) <<  i;
            if (currCv[i].prn >0)
            {
               out << "  Prn: " << setw(2) << currCv[i].prn
                   << "  Elev: " << fixed <<  setprecision(1) << setw(4) 
                   << currCv[i].elevation << " ";
               const rc_set &ccs = currCv[i].obs;
               for (rc_set::const_iterator j=ccs.begin(); j!=ccs.end(); j++)
                  out << " (" << asString(j->second)
                      << ", " << asString(j->first) << ")";
            }
            else
            {
               out << "  unused";
            }
            out << endl;
         }
      }
   }
   else
   {
      // This is the one line per epoch with changes
      bool change=false;
      for (int i = 1; i < currCv.size() && change==false; i++)
         change = (currCv[i].obs != prevCv[i].obs ||
                   currCv[i].prn != prevCv[i].prn) &&
            (prevCv[i].prn != -1 || currCv[i].prn != -1);

      if (change)
      {
         out << currTime.printf(timeFormat);
         for (int i = 1; i < currCv.size(); i++)
         {
            if (currCv[i].prn >0)
               out << setw(4) << currCv[i].prn << currCv[i].codes;
            else
               out << setw(4) << "  -" << "    ";
         }
         out << endl;
      }
   }
}
