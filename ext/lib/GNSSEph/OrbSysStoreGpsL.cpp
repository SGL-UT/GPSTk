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
 * @file OrbSysStoreGpsL.cpp - This is an extension of OrbSysStore that is
 *       specific to GPS LNAV.  It provides one additional method: an
 *       alternate version of addMessage( ) that accepts a PackedNavBits,
 *       determines the type of the contents (if they are GPS LNAV), 
 *       builds an appropriate OrbSysData descendent, and calls the 
 *       addMessage(const OrbSysData*) method in OrbSysStore.   
 *
 *       This was done in order to elminate the clutter of having all the
 *       message specific classes for all the nav message types embedded
 *       in OrbSysStore.  
 */
#include <iostream>

#include "OrbSysStoreGpsL.hpp"

#include "OrbSysGpsL_51.hpp"
#include "OrbSysGpsL_56.hpp"
#include "OrbSysGpsL_63.hpp"

using namespace std;

namespace gpstk
{

//------------------------------------------------------------------------------
// Convenience method.  Since most navigation message handling
// will likely involve PackedNavBits, we'll provide a means of 
// creating and storing a message based on a PackedNavBits.
   bool OrbSysStoreGpsL::addMessage(const PackedNavBits& pnb)
         throw(InvalidRequest,Exception)
   {
      if (debugLevel) cout << "Entering addMessage(PackedNavBits&)" << endl;
      
      const CommonTime& ct = pnb.getTransmitTime();
      const ObsID& oidr = pnb.getobsID();
      const SatID& sidr = pnb.getsatSys();

         // Translate SatID/ObsID to nav message type.
      unsigned long navtype = 1;       // Temporary expedient

         // Based on nav type, determine UID and convert 
         // PackedNavBits to OrbDataSys. 
      OrbDataSys* p = 0; 
      switch (navtype)
      {
            // GPS LNAV
         case 1:      
         {
            unsigned long sfNum = pnb.asUnsignedLong(49, 3, 1);
            if (sfNum!=4 && sfNum!=5) return false;
            unsigned long uid = pnb.asUnsignedLong(62,6,1);
            try
            {
               switch (uid)
               {
                     // Do nothing until the appropriate OrbSysGPSL_xx 
                     // classes are written.
                  case 51:
                  {
                     OrbSysGpsL_51* pnew = new OrbSysGpsL_51(pnb);
                     p = dynamic_cast<OrbDataSys*>(pnew);
                     break;
                  }

                  case 52:
                  case 53:
                  case 54:
                  case 55:
                  {
                     return false; 
                  }
               
                  case 56:
                  {
                     OrbSysGpsL_56* pnew = new OrbSysGpsL_56(pnb);
                     p = dynamic_cast<OrbDataSys*>(pnew);
                     break;
                  }
               
                  case 63:
                  {
                     OrbSysGpsL_63* pnew = new OrbSysGpsL_63(pnb);
                     p = dynamic_cast<OrbDataSys*>(pnew);
                     break;
                  }

                  default:
                  {
                     return false;  // Don't know what to do with this UID
                  }
               }
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
            break;
         }

            // Don't know what to do with this navigation message type.
         default:
         {
            return false; 
         }
      }

      bool retVal = false;
      
      if (p)
      {
         try
         {
            retVal = OrbSysStore::addMessage(p);
         }
         catch (InvalidRequest ir)
         {
            if (debugLevel)
            {
               cout << "Caught InvalidRequest" << endl;
               cout << ir << endl; 
            }
            GPSTK_RETHROW(ir);
         }
      }
      return retVal;
   }

} // namespace
