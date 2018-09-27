/// @file millisecHandler.cpp  Detect and fix millisecond clock adjusts in data,
///   including pseudoranges, phases and time tags.

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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

#include <vector>
#include <map>
#include <algorithm>
#include "GNSSconstants.hpp"
#include "TimeString.hpp"
#include "stl_helpers.hpp"          // for vectorindex
#include "Stats.hpp"                // for median
#include "StringUtils.hpp"

#include "msecHandler.hpp"

using namespace std;

namespace gpstk {

   // -------------------------------------------------------------------------------
   const double msecHandler::Rfact = 0.001*C_MPS;      // 1ms in meters

   // -------------------------------------------------------------------------------
   // empty and only constructor
   msecHandler::msecHandler(void)
   {
      dt = -1.0;
      N = 6;                     // default only - see setObstypes
      obstypes.push_back(string("L1")); wavelengths.push_back(L1_WAVELENGTH_GPS);
      obstypes.push_back(string("L2")); wavelengths.push_back(L2_WAVELENGTH_GPS);
      obstypes.push_back(string("C1")); wavelengths.push_back(0.0);
      obstypes.push_back(string("C2")); wavelengths.push_back(0.0);
      obstypes.push_back(string("P1")); wavelengths.push_back(0.0);
      obstypes.push_back(string("P2")); wavelengths.push_back(0.0);
      reset();
   }

   // -------------------------------------------------------------------------------
   // Reset the object
   void msecHandler::reset(void)
   {
      // don't reset dt
      prevttag = currttag = CommonTime::BEGINNING_OF_TIME;
      curr = vector< map<SatID, double> >(N);
      past = vector< map<SatID, double> >(N);
      ave = vector<double>(N,0.0);
      npt = vector<int>(N,0);

      typesMap.clear();
      findMsg = fixMsg = string();

      times.clear();
      nms.clear();
      ots.clear();
      adjMsgs.clear();
      badMsgs.clear();

      doPR = false;
      rmvClk = false;
   }

   // -------------------------------------------------------------------------------
   // define obstypes and wavelengths; NB set wavelength(code) = 0
   void msecHandler::setObstypes(const vector<string>& ots,
                                 const vector<double>& waves)
   {
      if(ots.size() != waves.size())
         GPSTK_THROW(Exception("Inconsistent input"));
      N = ots.size();
      obstypes = ots;
      wavelengths = waves;
      reset();
   }

   // -------------------------------------------------------------------------------
   // add data at one epoch. May be repeated at the same epoch, but MUST be done in
   // time order. NB assumes, as in RINEX, that data==0 means it is missing.
   void msecHandler::add(CommonTime ttag, SatID sat, string obstype, double data)
   {
      if(dt == -1.0)
         GPSTK_THROW(Exception("Must set nominal timestep first"));

      if(data == 0.0) return; // NB assumes, as in RINEX, that data==0 is missing.

      if(currttag == CommonTime::BEGINNING_OF_TIME)
         currttag = ttag;
      if(ttag != currttag)
         compute(ttag);

      // first find it in obstypes
      vector<string>::const_iterator it;
      it = find(obstypes.begin(),obstypes.end(),obstype);
      if(it == obstypes.end()) return;       // not one of the obstypes
      int i = (it - obstypes.begin());

      // store current value
      curr[i][sat] = data;
      // difference with past
      if(past[i].find(sat) != past[i].end()  // if past is there
            && curr[i][sat] != 0.0           // and curr is not zero
            && past[i][sat] != 0.0)          // and past is not zero
      {
         ave[i] += curr[i][sat] - past[i][sat];    // first difference
         npt[i]++;                                 // count it
      }
   }

   // -------------------------------------------------------------------------------
   // After all add() calls, and before calling fix()
   // @return number of fixes to apply
   int msecHandler::afterAddbeforeFix(void)
   {
      // compute adjusts based on all the saved data
      compute(CommonTime::END_OF_TIME);

      if(times.size() == 0) {
         fixMsg = string("No valid adjusts found - nothing to do");
         return 0;
      }
      if(times.size() == 1 && rmvClk) {
         rmvClk = false;
         fixMsg =
            string("Warning - cannot remove gross clock with only 1 ms adjust");
      }

      doPR = false;
      if(find(ots[0].begin(),ots[0].end(),string("C1")) != ots[0].end() ||
         find(ots[0].begin(),ots[0].end(),string("C2")) != ots[0].end() ||
         find(ots[0].begin(),ots[0].end(),string("P1")) != ots[0].end() ||
         find(ots[0].begin(),ots[0].end(),string("P2")) != ots[0].end()) {
            doPR = true;
            fixMsg += string("Adjusts applied to pseudorange, ")
                     + string("so apply fix to the timetags.");
      }
      else fixMsg += string("Do not apply adjusts to timtags.");

      ims = ntot = 0;
      tref = CommonTime::BEGINNING_OF_TIME;

      return times.size();
   }

   // -------------------------------------------------------------------------------
   // edit data by removing the millisecond adjusts, and optionally a piece-wise
   // linear model of the adjusts. Must be called in time order, as add() was.
   // NB may call repeatedly with the same ttag, however
   // NB ttag gets fixed every call, so don't keep calling with same variable ttag.
   void msecHandler::fix(CommonTime& ttag, SatID sat, string obstype, double& data)
   {
      // define the first linear clock
      if(rmvClk && tref == CommonTime::BEGINNING_OF_TIME) {
         tref = ttag;
         slope = double(nms[1])/(times[1]-times[0]);
         intercept = double(nms[0])-slope*(times[0]-ttag);
      }

      // advance to the next ms adjust?
      if(ims < times.size() && ::fabs(ttag-times[ims]) < 1.e-3) {
         ntot += nms[ims];
         fixMsg += string("\nFixed ") + adjMsgs[ims];
         ims++;
         if(rmvClk && ims < times.size()) {
            tref = times[ims-1];
            slope = double(nms[ims])/(times[ims]-tref);
            intercept = double(ntot);
         }  // else just leave them...extrapolation
      }

      // find index and wavelength for this obstype
      int index = vectorindex(obstypes,obstype);
      if(index == -1)
         GPSTK_THROW(Exception("Invalid obstype, internal error: "+obstype));
      double wl = wavelengths[index];

      // remove adjusts
      if(ims > 0 && ntot != 0) {
         // remove adjust from the time tag
         if(doPR) ttag -= ntot * 0.001;

         // remove adjust from the data
         if(find(ots[ims-1].begin(), ots[ims-1].end(), obstype) != ots[ims-1].end())
            if(data != 0.0) {
               data -= ntot * (wl == 0.0 ? Rfact : Rfact/wl);
            }
      }

      // remove gross (piece-wise linear) clock by adjusting time tags and all data
      if(rmvClk) {
         // compute the model at this time
         double dtot = (intercept + slope*(ttag-tref))*Rfact;
         ttag += dtot/C_MPS;
         if(wl != 0.0) dtot /= wl;
         if(data != 0.0) data += dtot;
      }
   }

   // -------------------------------------------------------------------------------
   // get messages generated during detection phase
   string msecHandler::getFindMessage(bool verbose)
   {
      size_t i;
      findMsg = string();

      findMsg += string("Searched for millisecond adjusts on obs types:");
      for(i=0; i<obstypes.size(); i++) findMsg += string(" ") + obstypes[i];
      findMsg += string("\n");
 
      findMsg += string("Millisecond adjusts: ")
         + StringUtils::asString(adjMsgs.size() + badMsgs.size())
         + string(" total adjusts found, ")
         + StringUtils::asString(badMsgs.size()) + string(" invalid");

      for(map<string,int>::iterator tit=typesMap.begin(); tit!=typesMap.end(); ++tit)
         findMsg += string("\n  Found ")
            + StringUtils::asString(tit->second) + string(" adjusts for ")
            + tit->first;

      if(typesMap.size() > 1)
         findMsg += string("\n  Warning - detected millisecond adjusts are not ")
                           + string("consistently applied to the observables.");

      if(adjMsgs.size() > 0 && badMsgs.size() > adjMsgs.size()/2)
         findMsg += string("\n  Warning - millisecond adjust detection seems to be ")
            + string("of poor quality - consider rerunning with option --noMS");

      if(verbose) {
         for(i=0; i<adjMsgs.size(); i++)
            findMsg += string("\n") + adjMsgs[i];
         for(i=0; i<badMsgs.size(); i++)
            findMsg += string("\n") + badMsgs[i];
      }

      return findMsg;
   }

   // -------------------------------------------------------------------------------
   // get map<CommonTime,int> of valid adjusts
   map<CommonTime, int> msecHandler::getAdjusts(void)
   {
      map<CommonTime, int> adjusts;
      for(unsigned int i=0; i<times.size(); i++)
         adjusts.insert(map<CommonTime,int>::value_type(times[i],nms[i]));

      return adjusts;
   }

   // -------------------------------------------------------------------------------
   // compute - pass it the upcoming ttag
   // NB. ineq1620.14o - trimble has 2 and 3 ms adjusts
   void msecHandler::compute(CommonTime ttag)
   {
      size_t i,j;
      int ii,in,nadj;
      static CommonTime lastttag(CommonTime::BEGINNING_OF_TIME);
      const static double mstol(0.2);

      if(prevttag != CommonTime::BEGINNING_OF_TIME) {
         // convert to millisecs and compute averages
         for(i=0; i<N; i++) {
            if(wavelengths[i] != 0.0) ave[i] *= wavelengths[i];
            if(npt[i] > 0)
               ave[i] *= 1000.0/(npt[i]*C_MPS);    // form average and convert to ms
            else
               ave[i] = 0.0;
         }

         // do for time tag as well
         double del = dt-(currttag-prevttag);
         del = ::fmod(del,dt);
         del *= 1000.0;

         // round to nearest integer ms
         vector<int> iave(N+1);                 // element [N] is timetag
         for(i=0; i<N; i++)                     // L1 L2 C1 C2 P1 P2
            iave[i] = int(ave[i]+(ave[i]>=0.0 ? 0.5:-0.5));
         iave[N] = (del+(del>0.0 ? 0.5:-0.5));  // N is timetag

         // test - is there an adjust? are the non-zero number-of-ms consistent?
         bool adj(false),consist(true),adjPh(false),adjPR(false);
         nadj = 0;                              // the adjust
         for(i=0; i<N; i++) {                   // test only the data, not timetags
            if(iave[i] != 0) {
               adj = true;
               if(wavelengths[i] != 0.0) adjPh = true; else adjPR = true;
               if(nadj == 0) nadj=iave[i];
               else if(nadj != iave[i]) consist = false;
            }
         }

         // treat phases specially - there can be large cycleslips that interfere
         // with determination of adjusts. these will be isolated to one sat, so
         // use robust stats to find the average (median).
         // TD consider median for all ave[]
         if(adjPh && adjPR) {
            bool foundPhase(false);
            for(i=0; i<N; i++) {                         // just phases
               if(wavelengths[i] == 0.0) continue;

               double med,mad;
               vector<double> deltas;
               map<SatID, double>::const_iterator it;

               // collect the differences, one per sat
               for(it = curr[i].begin(); it != curr[i].end(); ++it) {
                  if(past[i].find(it->first) != past[i].end()) {
                     // diff for this sat in ms
                     del = (curr[i][it->first] - past[i][it->first])
                                                        / (Rfact/wavelengths[i]);
                     deltas.push_back(del);
                  }
               }
               // get the median, an outlier will not affect this
               med = median<double>(deltas);
               // compute abs(deviation from median)
               for(j=0; j<deltas.size(); j++) deltas[j] = ::fabs(deltas[j]-med);
               // get median absolute deviation
               mad = median<double>(deltas);
               // replace average with median, which will be insensitive to outliers
               if(mad < 0.5) {
                  ave[i] = med;
                  iave[i] = int(ave[i]+(ave[i]>=0.0 ? 0.5:-0.5));
               }
               if(iave[i] != 0) foundPhase=true;
            }

            // fix it - duplicate code above
            if(!foundPhase) {
               adj = false; consist = true; adjPh = false; nadj = 0;
               for(i=0; i<N; i++) {
                  if(iave[i] != 0) {
                     adj = true;
                     if(nadj == 0) nadj=iave[i];
                     else if(nadj != iave[i]) consist = false;
                  }
               }
            }
         }

         // if there is an adjust, test it further, then store it
         if(adj) {
            //string conmsg(consist ? "" : (!isOne ? " adjust is not +-1ms" 
            string conmsg(consist ? "" : " adjust sizes are inconsistent");

            // test for shaky determination - adjust is not close to integer millisec,
            // and/or number of sats is low. do only if adjust is consistent
            double frac(::fabs(ave[0]-iave[0]));
            // TD should this include || npt[0] < 3 ?? yes tripwire brst 195
            if(frac > mstol || npt[0] < 3) {
               conmsg = string(" not well determined");
               consist = false;
            }

            // are they consistent? is phase consistent with code?
            ostringstream oss;
            in=-1; bool onphase(false);
            for(i=0; i<N; i++) {                         // phase only
               if(wavelengths[i] == 0.0) continue;       // skip PR
               if(npt[i] == 0) continue;                 // skip no data
               if(iave[i] != 0) onphase=true;
               if(in == -1) in=i;
               if(iave[i] != iave[in]) {
                  consist = false;
                  conmsg += string(" " + obstypes[in] +"!=" + obstypes[i]);
               }
            }
            in=-1; bool oncode(false);
            for(i=0; i<N; i++) {                         // code only
               if(wavelengths[i] != 0.0) continue;       // skip Phase
               if(npt[i] == 0) continue;                 // skip no data
               if(iave[i] != 0) oncode=true;
               if(in == -1) in=i;
               if(iave[i] != iave[in]) {
                  consist = false;
                  conmsg += string(" " + obstypes[in] +"!=" + obstypes[i]);
               }
            }
            if(consist && onphase && !oncode) conmsg += string(" (Phase-only)");
            if(consist && !onphase && oncode) conmsg += string(" (PR-only)");
            if(!consist) conmsg += string(" invalid");

            // create the types str, the ots and the msg
            oss.str("");
            string types;
            vector<string> no,ot;
            if(iave[N] == 0) no.push_back("TT");
            else             ot.push_back("TT");
            oss << (iave[N]==0 ? "!":"") << "TT";
            for(i=0; i<N; i++) {
               if(npt[i]) {
                  if(iave[i] == 0) no.push_back(obstypes[i]);
                  else             ot.push_back(obstypes[i]);
                  oss << " " << (iave[i]==0 ? "!":"") << obstypes[i];
               }
            }
            types = oss.str();

            // compute time since last adjust
            del = (lastttag != CommonTime::BEGINNING_OF_TIME ? currttag-lastttag:0.0);
            lastttag = currttag;

            // NB gps sow at .7 b/c RINEX time tags have this precision
            oss.str("");
            oss << "msAdjust "
               << printTime(currttag,"%04Y/%02m/%02d %02H:%02M:%06.3f = %4F %14.7g")
               << " dt=" << int(del+0.5) << " " << nadj << " ms " << types;
            oss << conmsg;
            
            if(consist) {                       // found a valid adjust
               // save
               nms.push_back(nadj);
               times.push_back(currttag);

               // save ots
               ots.push_back(ot);

               // increment types map
               if(typesMap.find(types) == typesMap.end())
                  typesMap.insert(map<string,int>::value_type(types,0));
               typesMap[types]++;

               adjMsgs.push_back(oss.str());

               // RinEdit commands
               for(i=0; i<N; i++) {
                  if(iave[i] == 0) continue;
                  oss.str("");
                  oss << "--BD+ ";
                  if(obstypes[i].length() > 2) {
                     oss << obstypes[i][0] << "," << obstypes[i].substr(1);
                  }
                  else {
                     oss << "G" << obstypes[i];
                  }
                  oss << printTime(currttag,",%F,%.3g");
                  oss << "," << fixed << setprecision(5) << - double(nadj)
                        * (wavelengths[i] == 0.0 ? Rfact : Rfact/wavelengths[i])
                        << " # edit cmd for " << nadj << " millisecond adjust";

                  editCmds.push_back(oss.str());
               }
            }
            else {                              // not a valid adjust
               badMsgs.push_back(oss.str());
            }
         }
      }  // end if prevttag != BEGIN

      // prepare for next epoch
      prevttag = currttag;
      currttag = ttag;
      for(i=0; i<N; i++) { past[i].clear(); past[i]=curr[i]; curr[i].clear(); }
      ave = vector<double>(N,0.0);
      npt = vector<int>(N,0);

   }  // end void compute(ttag)

} // end namespace
