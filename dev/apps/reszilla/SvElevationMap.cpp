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

#include "SvElevationMap.hpp"

using namespace std;
using namespace gpstk;

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
SvElevationMap elevation_map(const ObsEpochMap& oem,
                             const Triple& ap,
                             const EphemerisStore& eph)
{
   SvElevationMap pem;

   ECEF rxpos(ap);

   ObsEpochMap::const_iterator oem_itr;
   for (oem_itr=oem.begin(); oem_itr!=oem.end(); oem_itr++)
   {
      const DayTime& t = oem_itr->first;
      const ObsEpoch& oe = oem_itr->second;
      ObsEpoch::const_iterator oe_itr;
      for (oe_itr=oe.begin(); oe_itr!=oe.end(); oe_itr++)
         try
         {
            SatID prn = oe_itr->first;
            Xvt svpos = eph.getPrnXvt(prn.id, t);
            pem[t][prn] = rxpos.elvAngle(svpos.x);
         }
         catch (EphemerisStore::NoEphemerisFound& e)
         {
         }
   }
   return pem;
}
