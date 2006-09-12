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
 * @file BCEphemerisStore.cpp
 * Store GPS broadcast ephemeris information, and access by satellite and time
 */

#include <iostream>
#include <fstream>
#include <iomanip>

#include "StringUtils.hpp"
#include "BCEphemerisStore.hpp"
#include "MathBase.hpp"

using namespace std;
using namespace gpstk;
using gpstk::StringUtils::asString;

namespace gpstk
{
   //--------------------------------------------------------------------------
   //--------------------------------------------------------------------------
   const EngEphemeris&
   BCEphemerisStore::findEphemeris(SatID sat, const DayTime& t) 
      const throw(EphemerisStore::NoEphemerisFound)
   {
      try 
      {
         const EngEphemeris& eph
            = (method==0 ? findUserEphemeris(sat, t) : findNearEphemeris(sat,t) );
         return eph;
      }
      catch(NoEphemerisFound& nef)
      {
         GPSTK_RETHROW(nef);
      }
      catch(InvalidRequest& ir)
      {
         NoEphemerisFound nef(ir);
         GPSTK_THROW(nef);
      }
   }


   Xvt BCEphemerisStore::getSatXvt(SatID sat, const DayTime& t)
      const throw(EphemerisStore::NoEphemerisFound)
   {
      short ref;
      return getSatXvt(sat, t, ref);
   } // end of BCEphemerisStore::getSatXvt()


   Xvt BCEphemerisStore::getSatXvt(SatID sat, const DayTime& t, short& ref)
      const throw(EphemerisStore::NoEphemerisFound)
   {
      try
      {
         // test for GPS satellite system in sat?
         const EngEphemeris& eph = findEphemeris(sat,t);
         ref = eph.getIODC();
         Xvt sv = eph.svXvt(t);
         return sv;
      }
      catch(NoEphemerisFound& nef)
      {
         GPSTK_RETHROW(nef);
      }
      catch(InvalidRequest& ir)
      {
         NoEphemerisFound nef(ir);
         GPSTK_THROW(nef);
      }
   } // end of BCEphemerisStore::getSatXvt()


   //--------------------------------------------------------------------------
   //--------------------------------------------------------------------------
   short BCEphemerisStore::getSatHealth(SatID sat, const DayTime& t)
      const throw(EphemerisStore::NoEphemerisFound)
   {
      try
      {
         // test for GPS satellite system in sat?
         const EngEphemeris& eph = findEphemeris(sat, t);
         short health = eph.getHealth();
         return health;
      }
      catch(NoEphemerisFound& nef)
      {
         GPSTK_RETHROW(nef);
      }
      catch(InvalidRequest& ir)
      {
         NoEphemerisFound nef(ir);
         GPSTK_THROW(nef);
      }
   } // end of BCEphemerisStore::getHealth()

   //--------------------------------------------------------------------------
   //--------------------------------------------------------------------------
   void BCEphemerisStore::dump(short detail, std::ostream& s) const
   {
      UBEMap::const_iterator it;

      s << "Dump of BCEphemerisStore:\n";
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
   
         s << "  End of BCE maps." << std::endl << std::endl;
      }
   } // end of BCEphemerisStore::dump()


   //--------------------------------------------------------------------------
   // Only keeps one ephemeris with a given IODC/time
   // It should keep the one with the latest transmit time
   //--------------------------------------------------------------------------
   bool BCEphemerisStore::addEphemeris(const EngEphemeris& eph)
      throw(InvalidRequest)
   {
      bool rc = false;
      DayTime t(0.L);
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
         DayTime ephTot, currentTot;
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
  unsigned BCEphemerisStore::wiper(const DayTime& t) throw(InvalidRequest)
  {
     unsigned counter = 0;
     DayTime test;
     for(UBEMap::iterator i = ube.begin(); i != ube.end(); i++)
     {
        EngEphMap& thisPRN = i->second;
        EngEphMap::size_type mapSize = i->second.size();
        bool done = (i->second.size() == 0);
        while(!done)
        {
           EngEphemeris& foo = thisPRN.begin()->second;
           try
           {
              test = foo.getEphemerisEpoch();
           }
           catch (InvalidRequest& exc)
           {
              exc.addText("In wiping ephemerides for PRN " +
                          asString(i->first));
              exc.addText("Map had " + asString(mapSize) + " elements in"
                          " it to begin with");
              foo.dump(exc);
              GPSTK_RETHROW(exc);
           }
           if( test < t )
           {
              thisPRN.erase(thisPRN.begin());
              ++counter;
              done = (i->second.size() == 0);
           }
           else
           {
              done = true;
           }
        }
     }
        // update initialTime
     initialTime = t;
 
     return counter;
  }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
  void BCEphemerisStore::edit(const DayTime& tmin, const DayTime& tmax)
  {
     DayTime test;
     for(UBEMap::iterator i = ube.begin(); i != ube.end(); i++)
     {
        EngEphMap& eMap = i->second;

        EngEphMap::iterator lower = eMap.lower_bound(tmin);
        if (lower != eMap.begin())
           eMap.erase(eMap.begin(), --lower);

        EngEphMap::iterator upper = eMap.upper_bound(tmax);
        if (upper != eMap.end())
           eMap.erase(upper, eMap.end());
     }

     initialTime = tmin;
     finalTime = tmax;
  }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
   void BCEphemerisStore::clear() throw()
   {
      ube.clear();
      initialTime = DayTime::END_OF_TIME;
      finalTime = DayTime::BEGINNING_OF_TIME;
   }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
  unsigned BCEphemerisStore::ubeSize() const throw()
  {
    unsigned counter = 0;
    for(UBEMap::const_iterator i = ube.begin(); i != ube.end(); i++)
       counter += i->second.size();
    return counter;
  }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
   const EngEphemeris&
   BCEphemerisStore::findUserEphemeris(SatID sat, const DayTime& t) 
      const throw(EphemerisStore::NoEphemerisFound)
   {
      DayTime test;
      UBEMap::const_iterator prn_i = ube.find(sat.id);
      if (prn_i == ube.end())
      {
          NoEphemerisFound nef("No ephemeris for satellite " + asString(sat));
          GPSTK_THROW(nef);
      }

      const EngEphMap& em = prn_i->second;
      DayTime t1(0.0L), t2(0.0L), Tot = DayTime::BEGINNING_OF_TIME;
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
            + asString(sat) + " at " + t.printf("%03j %02H:%02M:%02S");
         NoEphemerisFound e(mess);
         GPSTK_THROW(e);
      }

      return it->second;
   } // end of BCEphemerisStore::findEphemeris()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
   const EngEphemeris&
   BCEphemerisStore::findNearEphemeris(SatID sat, const DayTime& t) 
      const throw(EphemerisStore::NoEphemerisFound)
   {
      DayTime test;
      UBEMap::const_iterator prn_i = ube.find(sat.id);
      if (prn_i == ube.end())
      {
          NoEphemerisFound nef("No ephemeris for satellite " + asString(sat));
          GPSTK_THROW(nef);
      }

      const EngEphMap& em = prn_i->second;
      double dt2min = -1;
      DayTime tstart, how;
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
            + asString(sat) + " at " + t.printf("%03j %02H:%02M:%02S");
         NoEphemerisFound e(mess);
         GPSTK_THROW(e);
      }

      return it->second;
   } // end of BCEphemerisStore::findNearEphemeris()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
   int BCEphemerisStore::addToList(std::list<EngEphemeris>& v) const throw()
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
   } // end of BCEphemerisStore::addToList(list<EngEphemeris>&)

} // namespace
