#pragma ident "$Id: $"

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


#include <StringUtils.hpp>
#include <Triple.hpp>
#include "MSCCalculator.hpp"

using namespace std;
using namespace gpstk::StringUtils;
using namespace gpstk;

namespace gpstk
{
      // the number of seconds in a year is 365 and a quarter days times
      // the number of seconds in a day
   const unsigned long MSCCalculator::SEC_YEAR = 
      (unsigned long)(365.25 * gpstk::DayTime::SEC_DAY);
   
   MSCCalculator& MSCCalculator::load(list<MSCData>& msclist) throw()
   {   
         // populate mscmap from msclist
         // this will let us do easy searches to find a requested station
         // by using the station id for the key
      list<MSCData>::const_iterator itr;
      for(itr = msclist.begin(); itr != msclist.end(); itr++)
      {
         mscmap[itr->station].push_back(*itr);
      }
      return *this;
   }
   
   ECEF MSCCalculator::getPosition(unsigned long station,
                              const gpstk::DayTime& time) const
      throw(gpstk::InvalidRequest)
   {
         // get the data for the correct station, if it isn't available
         // throw an exception
      map<unsigned long, list<MSCData> >::const_iterator stnItr;
      stnItr = mscmap.find(station);
      
      if (stnItr == mscmap.end())
      {
         string err = "Station coordinates for " +
            asString<unsigned long>(station) +
            " are not available.";
         gpstk::InvalidRequest e(err);
         GPSTK_THROW(e);
      }

         // get the data for the most recent by release date, that is
         // valid by effective epoch
      const MSCData* bestData = NULL;
      list<MSCData>::const_iterator dataItr;
      for (dataItr = stnItr->second.begin(); 
           dataItr != stnItr->second.end(); dataItr++)
      {
         if ( ( time >= dataItr->effepoch ) &&
              ( ( bestData == NULL ) || 
                ( dataItr->time > bestData->time ) || 
                ( ( dataItr->time == bestData->time ) && 
                  ( dataItr->effepoch > bestData->effepoch ) ) ) )
         {
            bestData = &*dataItr;
         }
      }
      
      if ( bestData == NULL )
      {
         string err = "Station coordinates for " +
            asString<unsigned long>(station) +
            " at " + time.asString() + " are not available.";
         gpstk::InvalidRequest e(err);
         GPSTK_THROW(e);
      }

         // calculate the time elapsed between reference time and
         // the time of interest to determine the total station drift
      double dt = (time - bestData->refepoch) / SEC_YEAR;
      ECEF staxyz = bestData->coordinates;
      const Triple& drift = bestData->velocities;
      
         // compute the position given the total drift vectors
      staxyz[0] += drift[0] * dt;
      staxyz[1] += drift[1] * dt;
      staxyz[2] += drift[2] * dt;

      return staxyz;
   }
}

      
