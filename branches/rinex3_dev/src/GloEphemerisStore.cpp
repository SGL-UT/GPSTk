#pragma ident "$Id"

/**
 * @file GloEphemerisStore.cpp
 * Get GLONASS ephemeris data information
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

#include "GloEphemerisStore.hpp"

namespace gpstk
{
   void GloEphemerisStore::addEphemeris(const Rinex3NavData& data)
      throw()
   {
      CommonTime t = data.time;
      SatID sat = data.sat;
      GloRecord&  glorecord = pe[sat][t];

      glorecord.x = ECEF(data.px,data.py,data.pz);
      glorecord.v = ECEF(data.vx,data.vy,data.vz);
      glorecord.a = ECEF(data.ax,data.ay,data.az);

      glorecord.dtime = data.TauN;
      glorecord.ddtime = data.GammaN;
      glorecord.MFtime = data.MFtime;
      glorecord.health = data.health;
      glorecord.freqNum = data.freqNum;
      glorecord.ageOfInfo = data.ageOfInfo;
      
      if (t<initialTime)
         initialTime = t;
      else if (t>finalTime)
         finalTime = t;
   }
}


