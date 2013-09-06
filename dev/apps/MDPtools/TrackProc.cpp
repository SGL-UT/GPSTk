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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2007, The University of Texas at Austin
//
//============================================================================

#include "Position.hpp"
#include "TrackProc.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


//-----------------------------------------------------------------------------
MDPTrackProcessor::MDPTrackProcessor(gpstk::MDPStream& in, std::ofstream& out)
   : MDPProcessor(in, out)
{
   timeFormat = "%02H:%02M:%04.1f";
   obsOut = true;
}


MDPTrackProcessor::~MDPTrackProcessor()
{}

void MDPTrackProcessor::process(const gpstk::MDPObsEpoch& oe)
{
   ChanMap::const_iterator j;
   if (oe.time != currTime)
   {
      printChanges();
      prevTime = currTime;
      currTime = oe.time;
      prevCv = currCv;
      currCv.clear();
   }

   int prn=oe.prn;
#pragma unused(prn)
   int chan=oe.channel;

   // make a set of the obs that this epoch has
   rc_set ccs;
   currCv[chan].codes = "" ;
   gpstk::MDPObsEpoch::ObsMap::const_iterator i;
   for (i = oe.obs.begin(); i != oe.obs.end(); i++)
   {
      const gpstk::MDPObsEpoch::Observation& obs=i->second;
      rcpair rcPair(obs.range, obs.carrier);
      ccs.insert(rcPair);
      if (obs.carrier == ccL1)
      {
         if      (obs.range == rcCA)       currCv[chan].codes += 'c';
         if      (obs.range == rcPcode)    currCv[chan].codes += 'p';
         else if (obs.range == rcYcode)    currCv[chan].codes += 'y';
         else if (obs.range == rcCodeless) currCv[chan].codes += 'z';
      }
      else if (obs.carrier == ccL2)
      {
         if      (obs.range == rcCM)       currCv[chan].codes += 'm';
         else if (obs.range == rcCL)       currCv[chan].codes += 'l';
         else if (obs.range == rcCMCL)     currCv[chan].codes += 'x';
         else if (obs.range == rcCA)       currCv[chan].codes += 'c';
         if      (obs.range == rcPcode)    currCv[chan].codes += 'p';
         else if (obs.range == rcYcode)    currCv[chan].codes += 'y';
         else if (obs.range == rcCodeless) currCv[chan].codes += 'z';
      }
      if (obs.carrier == ccL5)
      {
         if      (obs.range == rcI5)       currCv[chan].codes += 'i';
         else if (obs.range == rcQ5)       currCv[chan].codes += 'q';
      }
   }
   currCv[chan].obs = ccs;
   currCv[chan].prn = oe.prn;
   currCv[chan].elevation = oe.elevation;
}

void MDPTrackProcessor::printChanges()
{
   ChanMap::const_iterator i,j;
   i = currCv.begin();
   j = prevCv.begin();
   bool changed = true;
   while (i != currCv.end() && j != prevCv.end())
   {
      changed = i->first != j->first ||
         i->second.prn != j->second.prn ||
         i->second.codes != j->second.codes;
      if (changed)
         break;
      i++;
      j++;
   }

   if (!changed)
      return;
   
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
            out << printTime(currTime,timeFormat) << "  Ch:" << setw(2) <<  i;
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
      if (currCv.size() == 0)
         return;
      out << printTime(currTime,timeFormat) << "  ";
      for (i = currCv.begin(); i != currCv.end(); i++)
         out << setw(8) << i->second.prn << i->second.codes;
      out << endl;
   }
}
