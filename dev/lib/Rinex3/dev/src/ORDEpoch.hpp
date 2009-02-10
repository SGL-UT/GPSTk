#pragma ident "$Id: ORDEpoch.hpp 330 2006-12-01 15:58:14Z ocibu $"

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

/**
 * @file ORDEpoch.hpp
 * A set of observed range deviations for a single point in time from
 * a single receiver.
 */

#ifndef ORDEPOCH_HPP
#define ORDEPOCH_HPP

#include <map>
#include "Exception.hpp"
#include "ObsRngDev.hpp"
#include "ClockModel.hpp"
#include "SatID.hpp"

namespace gpstk
{
   class ORDEpoch
   {
   public:
      ORDEpoch() : wonky(false) {};

      /// defines a store for each SV's ord, indexed by prn
      typedef std::map<SatID, ObsRngDev> ORDMap;

      ORDEpoch& removeORD(const SatID& svid) throw()
      {
         ORDMap::iterator i = ords.find(svid);
         if(i != ords.end())
            ords.erase(i);
         return *this;
      }
   
      ORDEpoch& applyClockModel(const ClockModel& cm) throw()
      {
         if (cm.isOffsetValid(time))
         {
            clockOffset = cm.getOffset(time);
            removeOffset(clockOffset);
         }
         return *this;
      }

      ORDEpoch& removeOffset(const double offset) throw()
      {
         ORDMap::iterator i;
         for (i = ords.begin(); i != ords.end(); i++)
            i->second.applyClockOffset(offset);
         return *this;
      }

      vdouble clockOffset;    ///< clock bias value (application defined units)
      vdouble clockResidual;  ///< clock bias minus expected value
      ORDMap ords;            ///< map of ORDs in epoch
      gpstk::DayTime time;
      bool wonky;             ///< Indicates that this epoch is suspect

      friend std::ostream& operator<<(std::ostream& s, 
                                      const ORDEpoch& oe)
         throw()
      {
         s << "t=" << oe.time
           << " clk=" << oe.clockOffset << std::endl;
         ORDMap::const_iterator i;
         for (i=oe.ords.begin(); i!=oe.ords.end(); i++)
            s << i->second << std::endl;
         return s;
      }
   
   };

   // this is a store of ORDs over time
   typedef std::map<gpstk::DayTime, gpstk::ORDEpoch> ORDEpochMap;
}
#endif
