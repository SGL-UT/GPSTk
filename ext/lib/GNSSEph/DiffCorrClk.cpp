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
 * DiffCorrClk.cpp.
 * A single CNAV/CNAV-2 Clokc Differential Correction (CDC) packet
 * in engineering units.
 *
 */
#include <iomanip>

#include "DiffCorrClk.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"

using namespace std;

namespace gpstk
{
   //----------------------------------------------------------------
   DiffCorrClk::DiffCorrClk()
      :DiffCorrBase(),
       daf0(0.0),
       daf1(0.0),
       UDRA(0)
   {
   }

   //----------------------------------------------------------------
   DiffCorrClk::DiffCorrClk(const PackedNavBits& msg,
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
   void DiffCorrClk::loadData(const PackedNavBits& msg, 
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
      daf0 = msg.asSignedDouble(startBit+8,13,-35);
      daf1 = msg.asSignedDouble(startBit+21,8,-51);
      UDRA = msg.asLong(startBit+29,5,1);

      subjSv = SatID(subjPrnId,SatelliteSystem::GPS);

         // CNAV-2 corrections are always for CNAV-2.  In the 
         // case of CNAV, the data may be either for CNAv or LNAb.
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
   bool DiffCorrClk::isSameData(const DiffCorrClk& right) const
   {
      if (!DiffCorrBase::isSameData(right)) return false;
      if (daf0 !=right.daf0) return false;
      if (daf1 !=right.daf1) return false;
      if (UDRA !=right.UDRA) return false; 
      return true;
   }

   //----------------------------------------------------------------
   void DiffCorrClk::dump(std::ostream& s) const
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
      s << "daf0          " << setw(17) << daf0 << " sec" << endl;
      s << "daf1          " << setw(17) << daf1 << " sec/sec" << endl;
   }
}


