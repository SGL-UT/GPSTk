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

/**
 * @file GPSEphemerisStore.cpp
 * Store GPS broadcast ephemeris information, and access by satellite and time
 */

#include <iostream>
#include <fstream>
#include <iomanip>

#include "StringUtils.hpp"
#include "GPSEphemerisStore.hpp"
#include "MathBase.hpp"

using namespace std;
using namespace gpstk;
using gpstk::StringUtils::asString;

namespace Rinex3
{
   //--------------------------------------------------------------------------
   //--------------------------------------------------------------------------
   Xvt GPSEphemerisStore::getXvt(const SatID& sat, const CommonTime& t)
      const throw(InvalidRequest)
   {
      short ref;
      return getXvt(sat, t, ref);
   }


   //--------------------------------------------------------------------------
   //--------------------------------------------------------------------------
   Xvt GPSEphemerisStore::getXvt(const SatID& sat, const CommonTime& t, short& ref)
      const throw(InvalidRequest)
   {
      try
      {
         // test for GPS satellite system in sat?
         const EngEphemeris& eph = findEphemeris(sat,t);
         ref = eph.getIODC();
         Xvt sv = eph.svXvt(t);
         return sv;
      }
      catch(InvalidRequest& ir)
      {
         GPSTK_RETHROW(ir);
      }
   }


   //--------------------------------------------------------------------------
   //--------------------------------------------------------------------------
   const EngEphemeris&
   GPSEphemerisStore::findEphemeris(const SatID& sat, const CommonTime& t) 
      const throw(InvalidRequest)
   {
      try
      {
         return method==0 ? findUserEphemeris(sat, t) : findNearEphemeris(sat, t);
      }
      catch(InvalidRequest& ir)
      {
         GPSTK_RETHROW(ir);
      }
   }


   //--------------------------------------------------------------------------
   //--------------------------------------------------------------------------
   short GPSEphemerisStore::getSatHealth(const SatID& sat, const CommonTime& t)
      const throw(InvalidRequest)
   {
      try
      {
         // test for GPS satellite system in sat?
         const EngEphemeris& eph = findEphemeris(sat, t);
         short health = eph.getHealth();
         return health;
      }
      catch(InvalidRequest& ir)
      {
         GPSTK_RETHROW(ir);
      }
   } // end of GPSEphemerisStore::getHealth()


   //--------------------------------------------------------------------------
   //--------------------------------------------------------------------------
   void GPSEphemerisStore::dump(std::ostream& s, short detail) const
      throw()
   {
      UBEMap::const_iterator it;

      s << "Dump of GPSEphemerisStore:\n";
      if (detail==0)
      {
         unsigned bce_count=0;
         for (it = ube.begin(); it != ube.end(); it++)
            bce_count += it->second.size();

         s << " Span is " << initialTime
           << " to " << finalTime
           << " with " << bce_count << " entries."
           << std::endl;
      }
      else
      {
         for (it = ube.begin(); it != ube.end(); it++)
         {
            const EngEphMap& em = it->second;
            s << "  BCE map for satellite " << it->first
              << " has " << em.size() << " entries." << std::endl;
      
            EngEphMap::const_iterator ei;
            for (ei=em.begin(); ei != em.end(); ei++)
               if (detail==1)
                  s << "PRN " << setw(2) << it->first
                    << " TOE " << ei->second.getEpochTime()
                    << " TOC " << fixed << setw(10) << setprecision(3) << ei->second.getToc()
                    << " HOW " << setw(10) << ei->second.getHOWTime(2)
                    << " KEY " << ei->first
                    << std::endl;
               else
                  ei->second.dump();
         }
   
         s << "  End of GPSEphemerisStore data." << std::endl << std::endl;
      }
   }


   //--------------------------------------------------------------------------
   // Only keeps one ephemeris with a given IODC/time
   // It should keep the one with the latest transmit time
   //--------------------------------------------------------------------------
   bool GPSEphemerisStore::addEphemeris(const EngEphemeris& eph)
      throw()
   {
      bool rc = false;
      CommonTime t(0,0,0.0,GPS);  // use GPS for default
      t =  eph.getEphemerisEpoch();
      t -= 0.5*3600.0*eph.getFitInterval();
   
      EngEphMap& eem = ube[eph.getPRNID()];
      EngEphMap::iterator sfi = eem.find(t);
      if ( sfi == eem.end())
      {
         eem[t] = eph;
         rc = true;
      }
      else
      {
         // Store the new eph only if it has a later transmit time
         EngEphemeris& current = sfi->second;
         CommonTime ephTot, currentTot;
         ephTot = eph.getTransmitTime();
         currentTot = current.getTransmitTime();

         if (ephTot > currentTot)
         {
            //if (eph.getIODC() != current.getIODC())
            //cerr << "Wierd: prn:" << setw(2) << eph.getPRNID()
            //<< ", Toe:" << eph.getToe()
            //<< ", New IODC:" << eph.getIODC()
            //<< ", New TTx:" << eph.getTot()
            //<< ", Old IODC:" << current.getIODC()
            //<< ", Old TTx:" << current.getTot()
            //<< endl;
            
            current = eph;
            rc = true;
         }
      }

      // In any case, update the initial and final times
      if (t<initialTime)
         initialTime = t;
      else if (t>finalTime)
         finalTime = t;

      return rc;
   }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
   void GPSEphemerisStore::edit(const CommonTime& tmin, const CommonTime& tmax)
      throw()
   {
      for(UBEMap::iterator i = ube.begin(); i != ube.end(); i++)
      {
         EngEphMap& eMap = i->second;

         EngEphMap::iterator lower = eMap.lower_bound(tmin);
         if (lower != eMap.begin())
            eMap.erase(eMap.begin(), lower);

         EngEphMap::iterator upper = eMap.upper_bound(tmax);
         if (upper != eMap.end())
            eMap.erase(upper, eMap.end());
      }

      initialTime = tmin;
      finalTime = tmax;
   }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
   unsigned GPSEphemerisStore::ubeSize() const throw()
   {
      unsigned counter = 0;
      for(UBEMap::const_iterator i = ube.begin(); i != ube.end(); i++)
         counter += i->second.size();
      return counter;
   }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
   const EngEphemeris&
   GPSEphemerisStore::findUserEphemeris(const SatID& sat, const CommonTime& t) 
      const throw(InvalidRequest)
   {
      UBEMap::const_iterator prn_i = ube.find(sat.id);
      if (prn_i == ube.end())
      {
         InvalidRequest e("No ephemeris for satellite " + asString(sat));
         GPSTK_THROW(e);
      }

      const EngEphMap& em = prn_i->second;
      CommonTime t1(0,0,0.0,GPS), t2(0,0,0.0,GPS),
                 Tot = CommonTime::BEGINNING_OF_TIME;
      EngEphMap::const_iterator it = em.end();

      // Find eph with (Toe-(fitint/2)) > t - 4 hours
      // Use 4 hours b/c it's the default fit interval.
      // Backup one ephemeris to make sure you get the
      // correct one in case of fit intervals greater 
      // than 4 hours.
      EngEphMap::const_iterator ei = em.upper_bound(t - 4 * 3600); 
      if (!em.empty() && ei != em.begin() )
      {
         ei--;
      }
      
      for (; ei != em.end(); ei++)
      {
         const EngEphemeris& current = ei->second;
         // t1 = Toe-(fitint / 2)
         t1 = ei->first;
         // t2 = HOW time
         t2 = current.getTransmitTime();

         // Ephemeredes are ordered by fit interval.  
         // If the start of the fit interval is in the future, 
         // this and any more ephemerides are not the one you are
         // looking for.
         if( t1 > t ) 
         {
            break;
         }
         
         double dt1 = t - t1;
         double dt2 = t - t2;

         if (dt1 >= 0 &&                           // t is after start of fit interval
             dt1 < current.getFitInterval() * 3600 &&  // t is within the fit interval
             dt2 >= 0 &&                           // t is after Tot
             t2 > Tot )                            // this eph has the latest Tot
         {
            it = ei;
            Tot = t2;
         }
      }

      if (it == em.end())
      {
         string mess = "No eph found for satellite "
            + asString(sat) + " at " + (static_cast<CivilTime>(t)).printf("%02m/%02d/%04Y %02H:%02M:%02S");
         InvalidRequest e(mess);
         GPSTK_THROW(e);
      }

      return it->second;
   }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
   const EngEphemeris&
   GPSEphemerisStore::findNearEphemeris(const SatID& sat, const CommonTime& t) 
      const throw(InvalidRequest)
   {
      UBEMap::const_iterator prn_i = ube.find(sat.id);
      if (prn_i == ube.end())
      {
         InvalidRequest e("No ephemeris for satellite " + asString(sat));
         GPSTK_THROW(e);
      }

      const EngEphMap& em = prn_i->second;
      double dt2min = -1;
      CommonTime tstart, how;
      EngEphMap::const_iterator it = em.end();

      // Find eph with (Toe-(fitint/2)) > t - 4 hours
      // Use 4 hours b/c it's the default fit interval.
      // Backup one ephemeris to make sure you get the
      // correct one in case of fit intervals greater 
      // than 4 hours.
      EngEphMap::const_iterator ei = em.upper_bound(t - 4 * 3600); 
      if (!em.empty() && ei != em.begin() )
      {
         ei--;
      }
      
      for (; ei != em.end(); ei++)
      {
         const EngEphemeris& current = ei->second;
         // tstart = Toe-(fitint / 2)
         tstart = ei->first;
         // how = HOW time
         how = current.getTransmitTime();

         // Ephemerides are ordered by time of start of fit interval.  
         // If the start of the fit interval is in the future, 
         // this and any more ephemerides are not the one you are
         // looking for.
         if( tstart > t ) break;
         
         double dt1 = t - tstart;
         double dt2 = t - how;

         if (dt1 >= 0 &&                           // t is after start of fit interval
             dt1 <= current.getFitInterval()*3600 &&  // t is within the fit interval
             (dt2min == -1 || fabs(dt2) < dt2min))  // t is closest to HOW
         {
            it = ei;
            dt2min = fabs(dt2);
         }
      }

      if (it == em.end())
      {
         string mess = "No eph found for satellite "
            + asString(sat) + " at " + (static_cast<CivilTime>(t)).printf("%02m/%02d/%04Y %02H:%02M:%02S");
         InvalidRequest e(mess);
         GPSTK_THROW(e);
      }

      return it->second;
   }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
   int GPSEphemerisStore::addToList(std::list<EngEphemeris>& v) const throw()
   {
      int n=0;
      UBEMap::const_iterator prn_i;
      for (prn_i = ube.begin(); prn_i != ube.end(); prn_i++)
      {
         const EngEphMap& em = prn_i->second;
         EngEphMap::const_iterator ei;
         for (ei=em.begin(); ei != em.end(); ei++)
         {
            v.push_back(ei->second);
            n++;
         }
      }
      return n;
   }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//   const EngEphMap 
   const std::map<CommonTime, EngEphemeris>& 
   GPSEphemerisStore::getEphMap( const SatID& sat )
            const throw(InvalidRequest)
   {
      UBEMap::const_iterator prn_i = ube.find(sat.id);
      if (prn_i == ube.end())
      {
         InvalidRequest e("No ephemeris for satellite " + asString(sat));
         GPSTK_THROW(e);
      }
      return(prn_i->second);
   }
   
} // namespace
 
