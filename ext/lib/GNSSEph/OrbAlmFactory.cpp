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
#include <iostream>

#include "OrbAlmFactory.hpp"
#include "NavID.hpp"
#include "ObsID.hpp"
#include "SatID.hpp"

// GPS LNAV OrbAlm derived classes
#include "OrbAlmGen.hpp"
//#include "OrbAlmGlo.hpp"   // Future
//#include "OrbAlmReduced.hpp"  // Future

using namespace std;

namespace gpstk
{
   OrbAlmFactory::OrbAlmFactory()
      {};
   
   OrbAlm* OrbAlmFactory::
   convert(const gpstk::PackedNavBits& pnb)
      throw(InvalidParameter)
   {
      OrbAlm* retVal;
      const gpstk::SatID& sidr = pnb.getsatSys();
      const gpstk::ObsID& oidr = pnb.getobsID();

      NavID navID(sidr, oidr); 

      try
      {
      switch (navID.navType)
      {
         case NavID::ntGPSLNAV: 
         {
            retVal = GPSLNAV(pnb); 
            break;
         }

         case NavID::ntGPSCNAVL5: 
         case NavID::ntGPSCNAVL2:
         {
            retVal = GPSCNAV(pnb); 
            break;
         }

         case NavID::ntBeiDou_D1:
         { 
            retVal = BeiDou_D1(pnb); 
            break;
         }

         case NavID::ntBeiDou_D2:
         {
            retVal = BeiDou_D2(pnb); 
            break;
         }

         case NavID::ntGloCivilF: 
         {
            retVal = GloCivilF(pnb);
            break;
         }

         default: 
            break;
      }
      }
      catch (InvalidParameter ip)
      {
         GPSTK_RETHROW(ip); 
      }

      return retVal;
   }


   OrbAlm* OrbAlmFactory::
   GPSLNAV(const gpstk::PackedNavBits& pnb)
      throw(InvalidParameter)
   {
      OrbAlm* retVal = 0;

         // Determine the subframe and SV ID
      unsigned long sfNum = pnb.asUnsignedLong(49, 3, 1);
      if (sfNum!=4 && sfNum!=5) return retVal;
      unsigned long uid = pnb.asUnsignedLong(62,6,1);

         // If this is a Subframe 5, Page 25 (uid 51), then
         // pull the WNa/Toa and update the mostRecentToaLNAV.
         // Note that the existing mostRecentToaLNAV is fully
         // qualified, so one can use the full week number
         // from that CommonTime to resolve the 512 week ambiguity
         // in WNa.
      if (uid==51)
      {
         short WNa = (short) pnb.asUnsignedLong(76, 8, 1);
         unsigned long lToa = pnb.asUnsignedLong(68, 8, 4096);
         double Toa = (double) lToa; 
         short fullWN = static_cast<GPSWeekSecond>(pnb.getTransmitTime()).week;
         short fullWNa = EngNav::convertXBit(fullWN,WNa,EngNav::BITS8);
         OrbAlmGen::loadWeekNumber(fullWNa,Toa); 
      }

         // Determine whether the PNB object is an appropriate
         // OrbAlm candidate.   That is to say, is it 
         // in the range of SV ID 1-32.  
      if (uid<=32)
      {
         try
         {
            retVal = new OrbAlmGen(pnb);
         }
            // Conversion attempt failed.
         catch(InvalidParameter ir)
         {
            if (debugLevel)
            {
               cout << "Conversion attempt failed.  Caught an InvalidParameter" << endl;
               cout << ir << endl;
            }
            GPSTK_RETHROW(ir); 
         }
      }
      return retVal; 
   }

   OrbAlm* OrbAlmFactory::
   GPSCNAV(const gpstk::PackedNavBits& pnb)
      throw(InvalidParameter)
   {
      OrbAlm* retVal = 0;
      return retVal; 
   }

   OrbAlm* OrbAlmFactory::
   BeiDou_D1(const gpstk::PackedNavBits& pnb)
      throw(InvalidParameter)
   {
      OrbAlm* retVal = 0;
      return retVal; 
   }

   OrbAlm* OrbAlmFactory::
   BeiDou_D2(const gpstk::PackedNavBits& pnb)
      throw(InvalidParameter)
   {
      OrbAlm* retVal = 0;
      return retVal; 
   }

   OrbAlm* OrbAlmFactory::
   GloCivilF(const gpstk::PackedNavBits& pnb)
      throw(InvalidParameter)
   {
      OrbAlm* retVal = 0;
      return retVal; 
   }
   
}
 
