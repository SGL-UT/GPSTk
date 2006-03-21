#pragma ident "$Id: //depot/sgl/gpstk/dev/src/ObservationStore.cpp#3 $"

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
 * @file ObservationStore.cpp
 * A class encapsulating observation data (roughly standard RINEX obs and met files).
 */

#include "ObservationStore.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk
{
   void ObsEpoch::insertObservation( const Observation& inobs, 
                                     const short svId )
      throw()
   {
      obs[svId] = inobs;
   }
  
   void ObsEpoch::insertObservation( const ObservationPlus& op )
      throw()
   {
      insertObservation(op.obs, op.svId);
   }

   void RxObsData::insertObservation( const Observation& obs, 
                                      const DayTime& t, 
                                      const short svId )
      throw()
   {
      epochs[t].insertObservation(obs, svId);
      epochs[t].t = t;
      if (t > lastTime)  lastTime=t;
      if (t < firstTime) firstTime=t;
      prnList.insert(svId);
   }

   void RxObsData::insertObservation( const ObservationPlus& op )
      throw()
   {
      insertObservation(op.obs, op.time, op.svId);
   }

   void RxObsData::insertEpoch( const ObsEpoch& epoch )
      throw()
   {
      epochs[epoch.t] = epoch;
      epochs[epoch.t].t = epoch.t;
      if (epoch.t > lastTime)  lastTime  = epoch.t;
      if (epoch.t < firstTime) firstTime = epoch.t;

      ObsMap::const_iterator i;
      for(i=epoch.obs.begin(); i!=epoch.obs.end(); i++)
         prnList.insert(i->first);
   }

   WxObservation WxObsData::getMostRecent( const DayTime& t ) const
      throw()
   {
      if(obs.size() == 0)
         return WxObservation();
     
      WxObsMap::const_iterator i = obs.upper_bound(t);
      if (i== obs.end())
         i--;
      if (i != obs.begin())
         i--;
      return  i->second;
   };

   void WxObsData::insertObservation( const WxObservation& wx )
      throw()
   {
      obs[wx.t] = wx;
      if (wx.t > lastTime)  lastTime=wx.t;
      if (wx.t < firstTime) firstTime=wx.t;
   }

   bool WxObservation::isAllValid() const
      throw()
   {
      return temperatureSource != noWx
         && pressureSource != noWx
         && humiditySource != noWx;
   };

   void WxObsData::flush(const DayTime& t) throw()
   {
         // remove data from the WxObsMap
         // map is sorted by time, stop removing data at
         // first point after t
      WxObsMap::iterator i = obs.begin();
      while (i != obs.end())
      {
         if (i->first < t)
         {
            obs.erase(i);
            i = obs.begin();
            firstTime = i->second.t;
         }
         else
            break;
      }
   }

   WxObservation WxObsData::getWxObservation(const DayTime& t,
                                             unsigned iv,
                                             bool interpolate) const
      throw(ObjectNotFound)
   {
      if (obs.empty())
      {
         ObjectNotFound e("No WxObservation available near time " +
                          t.printf("%02H:%02M:%02S on day %03j of %4Y"));
         GPSTK_THROW(e);
      }

         // get the first object after time t;
      WxObsMap::const_iterator after = obs.upper_bound(t);
      
      if (after == obs.begin())
      {
         const WxObservation& wxa = after->second;
         if ((wxa.t >= (t - iv)) && (wxa.t <= (t + iv)))
         {
               // only after point fits
            return wxa;
         }
         else
         {
            ObjectNotFound e("No WxObservation available near time " +
                             t.printf("%02H:%02M:%02S on day %03j of %4Y"));
            GPSTK_THROW(e);
         }
      }
      

         // get the first object at or before time t;
      WxObsMap::const_iterator before = after;
      before--;

      if (after == obs.end())
      {
         const WxObservation& wxb = before->second;
         if((wxb.t >= (t - iv)) && (wxb.t <= (t + iv)))
         {
               // only before point fits
            return wxb;
         }
         else
         {
            ObjectNotFound e("No WeatherData available near time " +
                             t.printf("%02H:%02M:%02S on day %03j of %4Y"));
            GPSTK_THROW(e);
         }
      }
      else
      {
         const WxObservation& wxa = after->second;
         const WxObservation& wxb = before->second;

         if (interpolate)
         {
            if((wxb.t >= (t - iv)) && (wxb.t <= (t + iv)))
            {
               if ((wxa.t >= (t - iv)) && (wxa.t <= (t + iv)))
               {
                     // both points fit, linearly interpolate and create
                     // a WeatherData object with those values
                  double dtw = wxa.t - wxb.t;
                  double dt = t - wxb.t;

                  double slope = (wxa.pressure - wxb.pressure) / dtw;
                  double pressure = slope * dt + wxb.pressure;

                  slope = (wxa.humidity - wxb.humidity) / dtw;
                  double humidity = slope * dt + wxb.humidity;

                  slope = (wxa.temperature - wxb.temperature) / dtw;
                  double temp = slope * dt + wxb.temperature;

                  WxObservation wx(t, temp, pressure, humidity);
                  return wx;
               }
               else
               {
                     // only before point fits
                  return wxb;
               }
            }
            else if ((wxa.t >= (t - iv)) && (wxa.t <= (t + iv)))
            {
                  // only after point fits
               return wxa;
            }
            else
            {
               ObjectNotFound e("No WeatherData available near time " +
                                t.printf("%02H:%02M:%02S on day %03j of %4Y"));
               GPSTK_THROW(e);
            }
         }
         else
         {
            if((wxb.t >= (t - iv)) && (wxb.t <= (t + iv)))
            {
               if ((wxa.t >= (t - iv)) && (wxa.t <= (t + iv)))
               {
                     // both points fit, return closer point, or
                     // before point if at same distance
                  double diffa = wxa.t - t;
                  double diffb = t - wxb.t;
                  return(diffa < diffb ? wxa : wxb);
               }
               else
               {
                     // only before point fits
                  return wxb;
               }
            }
            else if ((wxa.t >= (t - iv)) && (wxa.t <= (t + iv)))
            {
                  // only after point fits
               return wxa;
            }
            else
            {
               ObjectNotFound e("No WeatherData available near time " +
                                t.printf("%02H:%02M:%02S on day %03j of %4Y"));
               GPSTK_THROW(e);
            }
         }
      }
   }

      // These are just to facilitate debugging.
   std::ostream& operator<<(std::ostream& s, 
                            const gpstk::Observation& obs)
      throw()
   {
      s << obs.range << ", " << obs.phase << ", " << obs.doppler
        << ", " << obs.SNR << ", " << obs.channel << ", ";
      switch (obs.carrier) {
         case gpstk::Observation::cfL1:    s << "L1 ";    break;
         case gpstk::Observation::cfL2:    s << "L2 ";    break;
         case gpstk::Observation::cfL5:    s << "L5 ";    break;
         case gpstk::Observation::cfOther: s << "Other "; break;
      }
      switch (obs.code) {
         case gpstk::Observation::rcCA:    s << "CA";    break;
         case gpstk::Observation::rcPY:    s << "PY";    break;
         case gpstk::Observation::rcZ:     s << "Z";     break;
         case gpstk::Observation::rcRC:    s << "RC";    break;
         case gpstk::Observation::rcOther: s << "Other"; break;
      }
      s << std::endl;
      return s;
   }

   std::ostream& operator<<(std::ostream& s, 
                            const gpstk::ObsEpoch& oe)
      throw()
   {
      s << "epoch t=" << oe.t << ", dt=" << oe.dt << std::endl;
      gpstk::ObsMap::const_iterator i;
      for (i=oe.obs.begin(); i!=oe.obs.end(); i++)
         s << "prn " <<  i->first << ", " << i->second;
      return s;
   }

   std::ostream& operator<<(std::ostream& s, 
                            const gpstk::RxObsData& rod)
      throw()
   {
      s << rod.rxId << " data spans " << rod.firstTime << " to " 
        << rod.lastTime << std::endl;
      gpstk::ObsEpochMap::const_iterator i;
      for (i=rod.epochs.begin(); i!=rod.epochs.end(); i++)
         s << "map t=" <<  i->first << ", " << i->second;
      return s;
   }

   std::ostream& operator<<(std::ostream& s, 
                            const gpstk::WxObservation& obs)
      throw()
   {
         // Note that this does not flag where the wx data came from
      s << obs.t << ", t=" << obs.temperature
        << ", p=" << obs.pressure
        << ", rh=" << obs.humidity;
      return s;
   }


}  // namespace
