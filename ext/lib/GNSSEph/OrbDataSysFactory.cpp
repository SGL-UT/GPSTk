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

#include "OrbDataSysFactory.hpp"
#include "NavID.hpp"
#include "ObsID.hpp"
#include "SatID.hpp"

// GPS LNAV OrbDataSys derived classes
#include "OrbSysGpsL_51.hpp"
#include "OrbSysGpsL_52.hpp"
#include "OrbSysGpsL_55.hpp"
#include "OrbSysGpsL_56.hpp"
#include "OrbSysGpsL_63.hpp"
#include "OrbSysGpsL_Reserved.hpp"

// GPS CNAV OrbDataSys derived classes
#include "OrbSysGpsC_30.hpp"
#include "OrbSysGpsC_32.hpp"
#include "OrbSysGpsC_33.hpp"

using namespace std;

namespace gpstk
{
   int OrbDataSysFactory::debugLevel;
   
   //----------------------------------------------------------------
   OrbDataSys* OrbDataSysFactory::
   convert(const gpstk::PackedNavBits& pnb)
      throw(InvalidParameter)
   {
      OrbDataSys* retVal;
      const gpstk::SatID& sidr = pnb.getsatSys();
      const gpstk::ObsID& oidr = pnb.getobsID();

      NavID navID(sidr, oidr); 

      try
      {
      switch (navID.navType)
      {
         case NavID::ntGPSLNAV: {retVal = GPSLNAV(pnb); break;}

         case NavID::ntGPSCNAVL5: 
         case NavID::ntGPSCNAVL2:
         {
            retVal = GPSCNAV(pnb); 
            break;
         }

         // Navigation message types other than GPS need to 
         // return 0 (a null pointer).  Someone else could choose
         // to implement a handler in an extension of this class. 
         default: 
            retVal = 0;
            break;
      }
      }
      catch (InvalidParameter ip)
      {
         GPSTK_RETHROW(ip); 
      }

      return retVal;
   }

   //----------------------------------------------------------------
   OrbDataSys* OrbDataSysFactory::
   GPSLNAV(const gpstk::PackedNavBits& pnb)
      throw(InvalidParameter)
   {
      OrbDataSys* retVal = 0;

         // Determine whether the PNB object is an appropriate
         // OrbDataSys candidate.   That is to say, is it 
         // in the range of SV ID 51-63.  
      unsigned long sfNum = pnb.asUnsignedLong(49, 3, 1);
      if (sfNum!=4 && sfNum!=5) return retVal;
      unsigned long uid = pnb.asUnsignedLong(62,6,1);
      try
      {
         switch (uid)
         {
            case 51:
            {
               retVal = new OrbSysGpsL_51(pnb);
               break;
            }

            case 52:
            {
               retVal = new OrbSysGpsL_52(pnb);
               break;
            }

            case 53:
            case 54:
            {
               retVal = new OrbSysGpsL_Reserved(pnb);
               break;
            }

            case 55:
            {
               retVal = new OrbSysGpsL_55(pnb);
               break;
            }
               
            case 56:
            {
                //OrbSysGpsL_56* pnew = new OrbSysGpsL_56(pnb);
                //retVal = dynamic_cast<OrbDataSys*>(pnew);
                retVal = new OrbSysGpsL_56(pnb);
                break;
            }

            case 57:
            case 58:
            case 59:
            case 60:
            case 61:
            case 62:
            {
               retVal = new OrbSysGpsL_Reserved(pnb);
               break;
            }

            case 63:
            {
               retVal = new OrbSysGpsL_63(pnb);
               break;
            }

            default:
            {
               // Do nothing
               break;
            }
         }         // end switch
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
      return retVal; 
   }

   //----------------------------------------------------------------
   OrbDataSys* OrbDataSysFactory::
   GPSCNAV(const gpstk::PackedNavBits& pnb)
      throw(InvalidParameter)
   {
      OrbDataSys* retVal = 0;

         // Determine whether the PNB object is an appropriate
         // OrbDataSys candidate.   That is to say, it has 
         // a MT in the range of SV ID 14-15, 30, 32-36.
         // NOTE: During the pre-operational test period, only a limited
         // set are implemented. 
      unsigned long uid = pnb.asUnsignedLong(14,6,1);
      try
      {
         switch (uid)
         {
            case 30:
            {
               retVal = new OrbSysGpsC_30(pnb);
               break;
            }
            case 32:
            {
               retVal = new OrbSysGpsC_32(pnb);
               break;
            }
            case 33:
            {
               retVal = new OrbSysGpsC_33(pnb);
               break;
            }
            {
               // Do nothing
               break;
            }
         }         // end switch
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
      return retVal; 
   }

}
 
