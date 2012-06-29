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
 * @file GalEphemerisStore.cpp
 * Store Galileo broadcast ephemeris information, and access by satellite and time
 */

#include <iostream>
#include <fstream>
#include <iomanip>

#include "StringUtils.hpp"
#include "GalEphemerisStore.hpp"
#include "MathBase.hpp"
#include "CivilTime.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;
using gpstk::StringUtils::asString;

namespace gpstk
{

//--------------------------------------------------------------------------

   Xvt GalEphemerisStore::getXvt(const SatID& sat, const CommonTime& t) const
      throw( InvalidRequest )
   {
      short ref;
      return getXvt(sat, t, ref);
   }

//--------------------------------------------------------------------------

   Xvt GalEphemerisStore::getXvt(const SatID& sat, const CommonTime& t, short& ref) const
      throw( InvalidRequest )
   {
      try
      {
         // test for Galileo satellite system in sat?
         const GalEphemeris& eph = findEphemeris(sat,t);

         // N.B. See above comment, same issue.
         //ref = eph.getIODC();
         
         Xvt sv = eph.svXvt(t);
         return sv;
      }
      catch(InvalidRequest& ir)
      {
         GPSTK_RETHROW(ir);
      }
   }

//--------------------------------------------------------------------------

   const GalEphemeris&
   GalEphemerisStore::findEphemeris(const SatID& sat, const CommonTime& t) const
      throw( InvalidRequest )
   {
      try
      {
         return strictMethod ? findUserEphemeris(sat, t) : findNearEphemeris(sat, t);
      }
      catch(InvalidRequest& ir)
      {
         GPSTK_RETHROW(ir);
      }
   }

//--------------------------------------------------------------------------

   short GalEphemerisStore::getSatHealth(const SatID& sat, const CommonTime& t) const
      throw( InvalidRequest )
   {
      try
      {
         // test for Galileo satellite system in sat?
         const GalEphemeris& eph = findEphemeris(sat, t);
         short health = eph.getHealth();
         return health;
      }
      catch(InvalidRequest& ir)
      {
         GPSTK_RETHROW(ir);
      }
   }

//--------------------------------------------------------------------------

   void GalEphemerisStore::dump(std::ostream& s, short detail) const
      throw()
   {
      UBEMap::const_iterator it;
      static const string fmt("%4F %10.3g = %04Y/%02m/%02d %02H:%02M:%02S %P");

      s << "Dump of GalEphemerisStore:\n";
      if (detail==0)
      {
         s << " Span is " << (initialTime == CommonTime::END_OF_TIME
                                      ? "End_time" : printTime(initialTime,fmt))
           << " to " << (finalTime == CommonTime::BEGINNING_OF_TIME
                                      ? "Begin_time" : printTime(finalTime,fmt))
           << " with " << ubeSize() << " entries."
           << std::endl;
      }
      else
      {
         for (it = ube.begin(); it != ube.end(); it++)
         {
            const GalEphMap& em = it->second;
            s << "  BCE map for satellite " << it->first
              << " has " << em.size() << " entries." << std::endl;
      
            GalEphMap::const_iterator ei;
            for (ei=em.begin(); ei != em.end(); ei++)
               if (detail==1)
                  s << "PRN " << setw(2) << it->first
                    << " TOE " << ei->second.getEpochTime()
                    << " TOC " << fixed << setw(10) << setprecision(3) << ei->second.getToc()
                    << " HOW " << setw(10) << ei->second.getHOWTime(2)
                    << " KEY " << ei->first
                    << std::endl;
               else
                  ei->second.dump(s);
         }
   
         s << "  End of GalEphemerisStore data." << std::endl << std::endl;
      }
   }

//--------------------------------------------------------------------------
// Keeps only one ephemeris with a given IODC/time.
// It should keep the one with the latest transmit time.
//--------------------------------------------------------------------------

   bool GalEphemerisStore::addEphemeris(const GalEphemeris& eph)
      throw()
   {
      bool rc = false;
      CommonTime t = eph.getEphemerisEpoch();

      // N.B. Galileo Nav Data does not define a fit interval.
      //      What should we replace this with?
      //t -= 0.5*3600.0*eph.getFitInterval();
   
      GalEphMap& eem = ube[eph.getPRNID()];
      GalEphMap::iterator sfi = eem.find(t);
      if ( sfi == eem.end())
      {
         eem[t] = eph;
         rc = true;
      }
      else
      {
         // Store the new eph only if it has a later transmit time
         GalEphemeris& current = sfi->second;

         if (eph.getTransmitTime() > current.getTransmitTime())
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
      if (rc)
      {
        if (t<initialTime)
          initialTime = t;
        else if (t>finalTime)
          finalTime = t;
      }

      return rc;
   }

//-----------------------------------------------------------------------------

   void GalEphemerisStore::edit(const CommonTime& tmin, const CommonTime& tmax)
      throw()
   {
      for(UBEMap::iterator i = ube.begin(); i != ube.end(); i++)
      {
         GalEphMap& eMap = i->second;

         GalEphMap::iterator lower = eMap.lower_bound(tmin);
         if (lower != eMap.begin())
            eMap.erase(eMap.begin(), lower);

         GalEphMap::iterator upper = eMap.upper_bound(tmax);
         if (upper != eMap.end())
            eMap.erase(upper, eMap.end());
      }

      initialTime = tmin;
      finalTime   = tmax;
   }

//-----------------------------------------------------------------------------

   unsigned GalEphemerisStore::ubeSize() const
      throw()
   {
      unsigned counter = 0;
      for(UBEMap::const_iterator i = ube.begin(); i != ube.end(); i++)
         counter += i->second.size();
      return counter;
   }

//-----------------------------------------------------------------------------

   const GalEphemeris&
   GalEphemerisStore::findUserEphemeris(const SatID& sat, const CommonTime& t) const
      throw( InvalidRequest )
   {
      UBEMap::const_iterator prn_i = ube.find(sat.id);
      if (prn_i == ube.end())
      {
         InvalidRequest e("No ephemeris for satellite " + asString(sat));
         GPSTK_THROW(e);
      }

      const GalEphMap& em = prn_i->second;
      CommonTime t1, t2, Tot = CommonTime::BEGINNING_OF_TIME;
      GalEphMap::const_iterator it = em.end();

      // Find eph with (Toe-(fitint/2)) > t - 4 hours
      // Use 4 hours b/c it's the default fit interval.
      // Backup one ephemeris to make sure you get the
      // correct one in case of fit intervals greater 
      // than 4 hours.
      GalEphMap::const_iterator ei = em.upper_bound(t - 4 * 3600); 
      if (!em.empty() && ei != em.begin() )
      {
         ei--;
      }
      
      for (; ei != em.end(); ei++)
      {
         const GalEphemeris& current = ei->second;
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
             // N.B. There is no fit interval. What should we replace this with?
             //dt1 < current.getFitInterval() * 3600 &&  // t is within the fit interval
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

   const GalEphemeris&
   GalEphemerisStore::findNearEphemeris(const SatID& sat, const CommonTime& t) const
      throw( InvalidRequest )
   {
      UBEMap::const_iterator prn_i = ube.find(sat.id);
      if (prn_i == ube.end())
      {
         InvalidRequest e("No ephemeris for satellite " + asString(sat));
         GPSTK_THROW(e);
      }

      const GalEphMap& em = prn_i->second;
      double dt2min = -1;
      CommonTime tstart, how;
      GalEphMap::const_iterator it = em.end();

      // Find eph with (Toe-(fitint/2)) > t - 4 hours
      // Use 4 hours b/c it's the default fit interval.
      // Backup one ephemeris to make sure you get the
      // correct one in case of fit intervals greater 
      // than 4 hours.
      GalEphMap::const_iterator ei = em.upper_bound(t - 4 * 3600); 
      if (!em.empty() && ei != em.begin() )
      {
         ei--;
      }
      
      for (; ei != em.end(); ei++)
      {
         const GalEphemeris& current = ei->second;
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
             // N.B. See previous method, same issue.
             //dt1 <= current.getFitInterval()*3600 &&  // t is within the fit interval
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

   int GalEphemerisStore::addToList(std::list<GalEphemeris>& v) const
      throw()
   {
      int n = 0;
      UBEMap::const_iterator prn_i;
      for (prn_i = ube.begin(); prn_i != ube.end(); prn_i++)
      {
         const GalEphMap& em = prn_i->second;
         GalEphMap::const_iterator ei;
         for (ei = em.begin(); ei != em.end(); ei++)
         {
            v.push_back(ei->second);
            n++;
         }
      }
      return n;
   }

//-----------------------------------------------------------------------------

   const GalEphemerisStore::GalEphMap&
   GalEphemerisStore::getEphMap( const SatID& sat ) const
      throw( InvalidRequest )
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
