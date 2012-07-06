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

#include "SvElevationMap.hpp"
#include "Position.hpp"

using namespace std;
using namespace gpstk;

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
SvElevationMap elevation_map(const ObsEpochMap& oem,
                             const Triple& ap,
                             const XvtStore<SatID>& eph)
{
   SvElevationMap pem;

   Position rxpos(ap);

   ObsEpochMap::const_iterator oem_itr;
   for (oem_itr=oem.begin(); oem_itr!=oem.end(); oem_itr++)
   {
      CommonTime t = oem_itr->first;
      t.setTimeSystem(TimeSystem::Any);
      const ObsEpoch& oe = oem_itr->second;
      ObsEpoch::const_iterator oe_itr;
      for (oe_itr=oe.begin(); oe_itr!=oe.end(); oe_itr++)
         try
         {
            SatID prn = oe_itr->first;
            Xvt svpos = eph.getXvt(prn, t);
            pem[t][prn] = rxpos.elvAngle(svpos.x);
         }
         catch (InvalidRequest& e)
         {
            cout << e << endl;
         }
   }
   return pem;
}
