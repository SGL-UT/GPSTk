//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#include "NavFilterKey.hpp"
#include "TimeString.hpp"

namespace gpstk
{
   NavFilterKey ::
   NavFilterKey()
         : stationID(""),
           rxID(""),
           carrier(CarrierBand::Unknown),
           code(TrackingCode::Unknown)
   {
   }

   void NavFilterKey::dump(std::ostream& s) const
   {
         // Use civil time format to accommodate multi-GNSS
      s << gpstk::printTime(timeStamp,"%02m/%02d/%4Y %02H:%02M:%04.1f ");
      s << std::setw(3) << prn << " " << stationID;
      if (rxID.length())
      {
         s << "/" << rxID;
      }
      s << " ";
      s << gpstk::ObsID::cbDesc[carrier] << ", " << gpstk::ObsID::tcDesc[code] << " "; 
   }

   std::ostream& operator<<(std::ostream& s, const NavFilterKey& nfk)
   {
      nfk.dump(s);
      return s; 
   }

/* don't use this...
   bool NavFilterKey ::
   operator<(const NavFilterKey& right) const
   {
      if (stationID < right.stationID) return true;
      if (stationID > right.stationID) return false;
      if (rxID < right.rxID) return true;
      if (rxID > right.rxID) return false;
      if (carrier < right.carrier) return true;
      if (carrier > right.carrier) return false;
      if (code < right.code) return true;
         //if (code > right.code) 
      return false;
   }
*/
}
