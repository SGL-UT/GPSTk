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

/**
 * @file ClockModel.cpp
 * Implement clock modeling for program DDBase.
 */

//------------------------------------------------------------------------------------
// includes
// system

// GPSTk

// DDBase
#include "DDBase.hpp"
#include "index.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// prototypes -- for this module only
int RemoveClockJumps(void) throw(Exception);             // here
int OutputClockData(void) throw(Exception);              // DataOutput.cpp

//------------------------------------------------------------------------------------
int ClockModel(void) throw(Exception)
{
try {
   int iret=0;

   if(CI.Verbose) oflog << "BEGIN ClockModel()"
      << " at total time " << fixed << setprecision(3)
      << double(clock()-totaltime)/double(CLOCKS_PER_SEC) << " seconds."
      ; //<< endl;
   oflog << " -- ClockModel() is not yet implemented." << endl;

   // TD remove discontinuitites in the clock model by looking at second differences
   // of the Station.ClockBuffer data. These are caused when the number of satellites
   // in the PR solution changes. Also remove an average by summing up the weighted
   // average jump removed.
   //iret = RemoveClockJumps();
   //if(iret) return iret;

   // output the clock data - Station.ClockBuffer and RxTimeOffset
   // this may be called just before abort in ReadAndProcessRawData, if PRS is far off
   OutputClockData();

   return iret;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end ClockModel()

//------------------------------------------------------------------------------------
int RemoveClockJumps(void) throw(Exception)
{
try {
   if(CI.Verbose) oflog << "BEGIN RemoveClockJumps()" << endl;

   bool jump;
   int n,iprev;
   size_t i;
   double curr,prev = 0,prevprev,sdiff,prevsdiff,frac,offset;
   CommonTime tt;
   map<int,double> jumps;
   map<string,Station>::iterator it;

   for(it=Stations.begin(); it != Stations.end(); it++) {
         // loop over epochs
      //offset = 0.0;
      jumps.clear();
      for(n=0,i=0; i<it->second.ClockBuffer.size(); i++) {
         curr = it->second.ClockBuffer[i];
         // when PRS fails, 0.0 is pushed into ClockBuffer
         if(curr == 0.0) continue;
         tt = FirstEpoch + it->second.CountBuffer[i]*CI.DataInterval;

         // second difference at (i) is (i)-2(i-1)+(i-2) ; ignore gaps in time
         if(n > 1) {
            jump = false;
            //const double fdiff = curr-prev;
            sdiff = curr-2*prev+prevprev;
            frac = 2*fabs(fabs(sdiff)-fabs(prevsdiff))/(fabs(sdiff)+fabs(prevsdiff));
            if(n>2 && fabs(sdiff)>0.3 && fabs(prevsdiff)>0.3
                   && sdiff*prevsdiff<0. && frac < 0.15) {
               jump = true;
               jumps[iprev] = prev-prevprev;
               oflog << "Define jump at " << iprev << endl;
               //offset += prev-prevprev;
            }
            //curr = it->second.ClockBuffer[i] += offset;

            //oflog << "C2D " << it->first << " " << tt.printf("%4F %10.3g")
            //   << fixed << setprecision(6)
            //   << " " << setw(10) << curr
            //   << " " << setw(10) << fdiff
            //   << " " << setw(10) << sdiff;
            //if(jump) oflog << " jump " << frac;
            //oflog << endl;

            prevsdiff = sdiff;
         }

         iprev = i;
         prevprev = prev;
         prev = curr;
         n++;

      }

      jump = false;
      offset = 0.0;
      map<int,double>::const_iterator kt = jumps.begin();
      for(n=0,i=0; i<it->second.ClockBuffer.size(); i++) {
         curr = it->second.ClockBuffer[i];
         if(curr == 0.0) continue;
         tt = FirstEpoch + it->second.CountBuffer[i]*CI.DataInterval;
         if(kt != jumps.end() && kt->first == (int)i) {
            oflog << "Found jump at " << i << endl;
            jump = true;
            offset += kt->second;
            kt++;
         }
         it->second.ClockBuffer[i] -= offset;

         //oflog << "C2DF " << it->first << " " << tt.printf("%4F %10.3g")
         //   << fixed << setprecision(6)
         //   << " " << setw(10) << curr
         //   << " " << setw(10) << curr-offset;
         //if(jump) { oflog << " jump " << setw(10) << offset; jump=false; }
         //oflog << endl;
      }
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end RemoveClockJumps()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
