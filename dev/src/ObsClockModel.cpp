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






/**
 * @file ObsClockModel.cpp
 * Yet another abstract class used to define an interface to a model that
 * accepts GPS observation datat and determines a clock model from it. It
 * mainly adds the ability to specify the characteristcs of the observations
 * that are to be accpeted into the model. It also defines a function that
 * accepts Observed Range Deviations and computes the mean of these (that
 * meet the selection criteria) as an estimate of the receiver clock.
 */

#include <math.h>

#include "ObsClockModel.hpp"

namespace gpstk
{
   using namespace std;

   ObsClockModel::PRNStatus ObsClockModel::getPRNStatus(short prn) const
      throw(gpstk::ObjectNotFound)
   {
      PRNStatusMap::const_iterator i = status.find(prn);
      if(i == status.end())
      {
         gpstk::ObjectNotFound e("No status for PRN " +
                                 gpstk::StringUtils::asString(prn) +
                                 " available.");
         GPSTK_THROW(e);
      }
      else
         return i->second;
   }

   ObsClockModel& ObsClockModel::setPRNModeMap(const PRNModeMap& right)
      throw()
   {
      for(int prn = 1; prn <= gpstk::MAX_PRN; prn++)
         modes[prn] = IGNORE;

      for(PRNModeMap::const_iterator i = right.begin(); i != right.end(); i++)
         modes[i->first] = i->second;

      return *this;
   }

   ObsClockModel::PRNMode ObsClockModel::getPRNMode(short prn) const
      throw(gpstk::ObjectNotFound)
   {
      PRNModeMap::const_iterator i = modes.find(prn);
      if(i == modes.end())
      {
         gpstk::ObjectNotFound e("No status for PRN " +
                                 gpstk::StringUtils::asString(prn) +
                                 " available.");
         GPSTK_THROW(e);
      }
      else
         return i->second;
   }


   gpstk::Stats<double> ObsClockModel::simpleOrdClock(const ORDEpoch& oe)
      throw(gpstk::InvalidValue)
   {
      gpstk::Stats<double> stat;
      
      status.clear();

      ORDEpoch::ORDMap::const_iterator itr;
      for(itr = oe.ords.begin(); itr != oe.ords.end(); itr++)
      {
         short prn = itr->first;
         const ObsRngDev& ord=itr->second;
         switch (modes[prn])
         {
            case IGNORE: 
               status[prn] = MANUAL;
               break;
            case ALWAYS:
               status[prn] = USED;
               break;
            case HEALTHY:
               // SV Health bits are defined in ICD-GPS-200C-IRN4 20.3.3.3.1.4
               // It is a 6-bit value where the MSB (0x20) indicates a summary of
               // of NAV data health where 0 = OK, 1 = some or all BAD
               if (ord.getHealth() & 0x20) 
                  status[prn] = SVHEALTH;
               break;
         }
      
         if (ord.getElevation() < elvmask)
            status[prn] = ELEVATION;

         if (status[prn] == USED)
            stat.Add(ord.getORD());
      }
   
      if (stat.N() > 2)
      {
         for (itr = oe.ords.begin(); itr != oe.ords.end(); itr++)
         {
            short prn = itr->first;

            // don't override other types of stripping
            if (status[prn] == USED)
            {
               // get absolute distance of residual from mean
               double res = itr->second.getORD();
               double dist = res - stat.Average();
               if(fabs(dist) > (sigmam * stat.StdDev()))
                  status[prn] = SIGMA;
            }
         }
   
         // now, recompute the statistics on unstripped residuals to get
         // the clock bias value
         stat.Reset();
         for (itr = oe.ords.begin(); itr != oe.ords.end(); itr++)
            if (status[itr->second.getPRN()] == USED)
               stat.Add(itr->second.getORD());
      
      }
      return stat;
   }

   void ObsClockModel::dump(ostream& s, short detail) const throw()
   {
      s << "min elev:" << elvmask
        << ", max sigma:" << sigmam
        << ", prn/status: ";
      
      ObsClockModel::PRNStatusMap::const_iterator i;
      for ( i=status.begin(); i!= status.end(); i++)
         s << i->first << "/" << i->second << " ";
   }
}

