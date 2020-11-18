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

/**
 * DiffCorrEph.cpp.
 * A single CNAV/CNAV-2 Ephemeris Differential Correction (EDC) packet
 * in engineering units.
 */
#include <iomanip>

#include "DiffCorrEph.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"

using namespace std;

namespace gpstk
{
   //----------------------------------------------------------------
   DiffCorrEph::DiffCorrEph()
      :DiffCorrBase(),
       dalpha(0.0),
       dbeta(0.0),
       dgamma(0.0),
       di(0.0),
       dOMEGA(0.0),
       dA(0.0),
       dUDRA(0)
   {
   }

   //----------------------------------------------------------------
   DiffCorrEph::DiffCorrEph(const PackedNavBits& msg,
                            const unsigned startBit)
   {
      try
      {
         loadData(msg,startBit);
      }
      catch (InvalidParameter ip)
      {
         GPSTK_RETHROW(ip); 
      }
   }

   //----------------------------------------------------------------
   void DiffCorrEph::loadData(const PackedNavBits& msg, 
                            const unsigned startBit)
   {
      try
      {
         DiffCorrBase::loadData(msg,startBit);
      }
      catch (InvalidParameter ip)
      {
         GPSTK_RETHROW(ip); 
      }

      unsigned short subjPrnId = msg.asUnsignedLong(startBit,8,1);
      dalpha = msg.asSignedDouble(startBit+8,14,-34);
      dbeta = msg.asSignedDouble(startBit+22,14,-34);
      dgamma = msg.asDoubleSemiCircles(startBit+36,15,-32);
      di = msg.asDoubleSemiCircles(startBit+51,12,-32);
      dOMEGA = msg.asDoubleSemiCircles(startBit+63,12,-32);
      dA = msg.asSignedDouble(startBit+75,12,-9);
      dUDRA = msg.asLong(startBit+87,5,1);

      subjSv = SatID(subjPrnId,SatelliteSystem::GPS);

         // CNAV-2 corrections are always for CNAV-2.  In the 
         // case of CNAV, the data may be either for CNAV or LNAV.
         // This is indicted in the DC Data Type bit that is 
         // just prior to the start of the packet.   See 
         // IS-GPS-200 Section 30.3.3.7.1
      if (msg.getNumBits()==274)   // Length of a CNAV-2 message is different then CNAV.
         dcDataType = dtCNAV2;
      else
      {
         unsigned DCType = msg.asUnsignedLong(startBit-1,1,1);
         if (DCType==0)
            dcDataType = dtCNAV;
          else
            dcDataType = dtLNAV;
      }

      dataLoadedFlag = true; 
   } 
  
   //----------------------------------------------------------------
   bool DiffCorrEph::isSameData(const DiffCorrEph& right) const
   {
      if (!DiffCorrBase::isSameData(right)) return false;
      if (dalpha !=right.dalpha) return false;
      if (dbeta  !=right.dbeta)  return false;
      if (dgamma !=right.dgamma) return false;
      if (di     !=right.di)     return false;
      if (dOMEGA !=right.dOMEGA) return false;
      if (dA     !=right.dA)     return false;
      if (dUDRA  !=right.dUDRA)  return false; 
      return true;
   }

   //----------------------------------------------------------------
   void DiffCorrEph::dump(std::ostream& s) const
   {
      if (!dataLoadedFlag)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }    
      DiffCorrBase::dump();
      s.setf(ios::scientific, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(10);
      s.fill(' ');
      s << "dalpha     " << setw(17) << dalpha << " (dimensionless)" << endl;
      s << "dbeta      " << setw(17) << dbeta << " (dimensionless)" << endl;
      s << "dgamma     " << setw(17) << dgamma << " radians" << endl;
      s << "di         " << setw(17) << di << " radians" << endl;
      s << "dOMEGA     " << setw(17) << dOMEGA << " radians" << endl;
      s << "dA         " << setw(17) << dA << " radians" << endl;
   }
}


