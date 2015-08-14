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
 * @file EphemerisImprovement.cpp
 * Implement improvements to the input satellite ephemerides, part of program DDBase.
 */

//------------------------------------------------------------------------------------
// system includes
#include <list>

// GPSTk

// DDBase
#include "DDBase.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// prototypes -- this module only

//------------------------------------------------------------------------------------
int EphemerisImprovement(void) throw(Exception)
{
try {
   if(CI.Verbose) oflog << "BEGIN EphemerisImprovement()"
      << " at total time " << fixed << setprecision(3)
      << double(clock()-totaltime)/double(CLOCKS_PER_SEC) << " seconds."
      << endl;

   if(dynamic_cast<GPSEphemerisStore*>(pEph)) {

      GPSEphemerisStore& BCE = dynamic_cast<GPSEphemerisStore&>(*pEph);
      list<GPSEphemeris> EphList;
      long i;

      oflog << "EphemerisStore is broadcast ephemeris" << endl;
      //BCE.dump(1,oflog);

      (void)BCE.addToList(EphList);

      GSatID sat;
      map<GSatID,long> IODEmap;
      map<GSatID,long>::iterator Imapit;
      list<GPSEphemeris>::const_iterator it;
      // dump the list of ephemerides, and build a map of sat,IODE
      for(i=0,it=EphList.begin(); it != EphList.end(); it++,i++) {
         short prn = it->satID.id;
         sat = GSatID(prn,SatID::systemGPS);
         if(IODEmap.find(sat) == IODEmap.end()) {
            IODEmap[sat] = -1;
         }
         //oflog << " " << setw(2) << i+1
         //      << " " << sat
         //      << " TOE: " << it->getEphemerisEpoch()
         //      << " TOC: " << it->getEpochTime()
         //      << " HOW: " << it->getTransmitTime()
         //      << " IODE: " << it->getIODE()
         //      << endl;
      }

      //for(Imapit=IODEmap.begin(); Imapit != IODEmap.end(); Imapit++) {
      //   oflog << " IODEmap[" << Imapit->first << "] = " << Imapit->second << endl;
      //}

      // test change of ephemeris during the time span of the data
      CommonTime tt=FirstEpoch;
      do {
         
         for(Imapit=IODEmap.begin(); Imapit != IODEmap.end(); Imapit++) {
            sat = Imapit->first;
            try {
               //const EngEphemeris& eph = BCE.findNearEphemeris(sat.prn,tt);
               // findNearEphemeris is obsolete b/c when an ephemeris is updated
               // early, its fit interval is adjusted to start (usually 16 seconds)
               // BEFORE that of the ephemeris it is replacing.
               // This means findEphemeris will now always find the 'best' ephemeris,
               // whereas findNearEphemeris used to fill that purpose.
               const GPSEphemeris& eph = BCE.findEphemeris(sat,tt);
               i = eph.IODE;
               if(IODEmap[sat] == -1) {
                  //oflog << "Satellite " << sat
                  //      << " starts with ephemeris IODE " << i
                  //      << " at epoch " << tt << endl;
                  IODEmap[sat] = i;
               }
               if(IODEmap[sat] != i) {
                  //oflog << "Satellite " << sat
                  //    << " changes ephemeris (" << IODEmap[sat]
                  //    << " -> " << i << ") at epoch "
                  //    << tt.printf("%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g") << endl;
                  IODEmap[sat] = i;
               }
            }
            catch(InvalidRequest& nef) { continue; }
         }

         tt += CI.DataInterval;

      } while(tt < LastEpoch);
      
   }

   if(dynamic_cast<SP3EphemerisStore*>(pEph)) {
      oflog << "EphemerisStore is SP3 ephemeris" << endl;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end EphemerisImprovement()
