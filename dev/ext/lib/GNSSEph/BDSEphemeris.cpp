/// @file BDSEphemeris.cpp Encapsulates the GPS legacy broadcast ephemeris and clock.
/// Inherits OrbitEph, which does most of the work; this class adds health and
/// accuracy information, fit interval, ionospheric correction terms and data
/// flags.

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

#include <string>
#include "Exception.hpp"
#include "SVNumXRef.hpp"
#include "BDSWeekSecond.hpp"

#include "BDSEphemeris.hpp"
#include "TimeString.hpp"

using namespace std;

namespace gpstk
{
   // Returns true if the time, ct, is within the period of validity of
   // this OrbitEph object.
   // @throw Invalid Request if the required data has not been stored.
   bool BDSEphemeris::isValid(const CommonTime& ct) const
   {
      try {
         if(ct >= beginValid && ct <= endValid) return true;
         return false;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   // This function returns the health status of the SV.
   bool BDSEphemeris::isHealthy(void) const
   {
      try {
         OrbitEph::isHealthy();     // ignore the return value; for dataLoaded check
         if(health == 0) return true;
         return false;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   // adjustBeginningValidity determines the beginValid and endValid times.
   // Note that this is currently a "best guess" based on observation of Beidou
   // operation. The concept of a fit interval is mentioned in the ICD, but the 
   // fit interval is undefined. 
   //   - It appears the Toe is aligned with the beginning of transmit.
   //   - It is assumed data should not be used prior to transmit.  
   //   - The transmission period appears to be one hour.
   //   - It is assumed that the data will be good for another hour
   //     in order to support SV position determination for 
   //     users that cannot collect navigation message continuously.
   // @throw Invalid Request if the required data has not been stored.
   void BDSEphemeris::adjustValidity(void)
   {
      try {
         OrbitEph::adjustValidity();   // for dataLoaded check
         beginValid = ctToe;
         endValid = ctToe + 3600.0;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }
      
   // Dump the orbit, etc information to the given output stream.
   // @throw Invalid Request if the required data has not been stored.
   void BDSEphemeris::dumpBody(std::ostream& os) const
   {
      try {
         OrbitEph::dumpBody(os);

         os << "           BeiDou-SPECIFIC PARAMETERS\n"
            << scientific << setprecision(8)
            << "Tgd (B1/B3) : " << setw(16) << Tgd13 << " meters" << endl
            << "Tgd (B2/B3) : " << setw(16) << Tgd23 << " meters" << endl
            << "HOW time    : " << setw(6) << HOWtime << " (sec of BDS week "
               << setw(4) << static_cast<BDSWeekSecond>(ctToe).getWeek() << ")"
            << "   fitDuration: " << setw(2) << fitDuration << " hours" << endl
            << "TransmitTime: " << OrbitEph::timeDisplay(transmitTime) << endl
            << "Accuracy    : " << fixed << setprecision(2)
            << getAccuracy() << " meters" << endl
            << "IODC: " << IODC << "   IODE: " << IODE << "   health: " << health
            << endl;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   void BDSEphemeris::dumpTerse(std::ostream& os) const
   {
      string tform = "%03j %02H:%02M:%02S";
      try
      {
	 os << " " << setw(3) << satID.id << " ! ";     
         os << printTime(transmitTime,tform) << " ! " 
	    << printTime(ctToe,tform) << " ! "
	    << printTime(endValid,tform) << " !"
	    << fixed << setprecision(2)
	    << setw(6) << getAccuracy() << "!"
	    << setw(4) << IODC << "!"
	    << setw(4) << IODE << "!"
	    << setw(6) << health << "!" << endl;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }


} // end namespace
