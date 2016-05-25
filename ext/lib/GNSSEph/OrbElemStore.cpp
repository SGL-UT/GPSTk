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

/**
 * @file OrbElemStore.cpp
 * Store GNSS broadcast OrbElemBase information, and access by 
 * satellite and time.  Several of the "least common denominator"
 * methods are defined in this base class, several are over-ridden
 * by descendent classes. 
 */

#include <iostream>
#include <fstream>
#include <iomanip>

#include "StringUtils.hpp"
#include "OrbElemStore.hpp"
#include "MathBase.hpp"
#include "CivilTime.hpp"
#include "TimeString.hpp"

using namespace std;
using gpstk::StringUtils::asString;


namespace gpstk
{

//--------------------------------------------------------------------------

   Xvt OrbElemStore::getXvt(const SatID& sat, const CommonTime& t) const
      throw( InvalidRequest )
   {
      try
      {
         // Find appropriate orbit elements (if available)
         const OrbElemBase* eph = findOrbElem(sat,t);

         // If the orbital elements are unhealthy, refuse to 
         // calculate an SV position and throw.
         if (!eph->healthy)
         {
            InvalidRequest exc( std::string("SV is transmitting unhealhty navigation ")
                + std::string("message at time of interest.") );
            GPSTK_THROW( exc );
         }
         Xvt sv = eph->svXvt(t);
         return sv;
      }
      catch(InvalidRequest& ir)
      {
         GPSTK_RETHROW(ir);
      }
   }

//------------------------------------------------------------------------------

   void OrbElemStore::validSatSystem(const SatID& sat) const 
      throw( InvalidRequest )
   {
      if (!isSatSysPresent(sat.system))
      {
         stringstream ess;
         ess << "Store does not contain orbit/clock elements for system ";
         ess << sat.system;
         ess << ". " << endl;
         ess << " Valid systems are :" << endl;
         
         list<SatID::SatelliteSystem>::const_iterator cit;
         for (cit=sysList.begin();cit!=sysList.end();cit++)
         {
            SatID::SatelliteSystem ssTest = *cit;
            ess << SatID::convertSatelliteSystemToString(ssTest) << endl;
         }
         
         InvalidRequest ir(ess.str());
         GPSTK_THROW(ir);
      }
   }

//--------------------------------------------------------------------------

   bool OrbElemStore::isHealthy(const SatID& sat, const CommonTime& t) const
      throw( InvalidRequest )
   {
      try
      {
         validSatSystem(sat);

         // Find appropriate orbit elements (if available)
         const OrbElemBase* eph = findOrbElem(sat, t);
         
         return eph->isHealthy();
      }
      catch(InvalidRequest& ir)
      {
         GPSTK_RETHROW(ir);
      }
   } // end of OrbElemStore::getHealth()

//--------------------------------------------------------------------------
// Typically overridden by descendents to obtain system-specific 
// listing behavior.

   void OrbElemStore::dump(std::ostream& s, short detail) const
      throw()
   {
      UBEMap::const_iterator it;
      static const string fmt("%04Y/%02m/%02d %02H:%02M:%02S %P");

      s << "Dump of OrbElemStore:\n";
      if (detail==0)
      {
         s << " Span is " << (initialTime == CommonTime::END_OF_TIME
                                      ? "End_time" : printTime(initialTime,fmt))
           << " to " << (finalTime == CommonTime::BEGINNING_OF_TIME
                                      ? "Begin_time" : printTime(finalTime,fmt))
           << " with " << size() << " entries."
           << std::endl;
      } // end if-block
      else if (detail==1)
      {
         for (it = ube.begin(); it != ube.end(); it++)
         {
            const OrbElemMap& em = it->second;
            s << "  Orbit/clock list for satellite " << it->first
              << " has " << em.size() << " entries." << std::endl;
            OrbElemMap::const_iterator ei;

            for (ei = em.begin(); ei != em.end(); ei++) 
            {
               const OrbElemBase* oe = ei->second;
               s << "PRN " << setw(2) << it->first
                 << " TOE " << printTime(oe->ctToe,fmt)
                 << " KEY " << printTime(ei->first,fmt);
               s << " begVal: " << printTime(oe->beginValid,fmt)
                 << " endVal: " << printTime(oe->endValid,fmt); 
                
               s << std::endl;
            } //end inner for-loop */

         } // end outer for-loop
   
         //s << "  End of Orbit/Clock data." << std::endl << std::endl;

      } //end else-block
   } // end OrbElemStore::dump

//------------------------------------------------------------------------------------
// Keeps only one OrbElemBase for a given SVN and Toe.
// It should keep the one with the earliest transmit time.
//------------------------------------------------------------------------------------ 
   bool OrbElemStore::addOrbElem(const OrbElemBase* eph)
      throw(InvalidParameter,Exception)
   {
     bool dbg = false;
     //if (eph->satID.id==2 ||
     //    eph->satID.id==5) dbg = true;
     
     try
     {
     SatID sid = eph->satID;
     OrbElemMap& oem = ube[sid];
     string ts = "%02m/%02d/%02y %02H:%02M:%02S";

     if (dbg)
     {
         cout << "Entering OrbElemStore::addOrbElem." << endl;
         cout << "   Toe = " << printTime(eph->ctToe,ts) << endl;
     }

       // If satellite system is wrong type for this store, throw an error
     if (!isSatSysPresent(sid.system))
     {
        stringstream ess;
        ess << "Attempted to add orbit elements for satellite";
        ess << sid;
        ess << " and that satellite system is not contained in this store.";
        InvalidParameter ip(ess.str());
        GPSTK_THROW(ip);
     }

       // if map is empty, load object and return
     if (dbg) cout << "oes::addOE.  Checking oem.size()" << endl;
     if (oem.size()==0)
     {
        oem[eph->beginValid] = eph->clone();
        updateInitialFinal(eph);
        return (true);
     }
       // Search for beginValid in current keys.
       // If found candidate, should be same data
       // as already in table. Test this by using the
       // isSameData() method.
     if (dbg) cout << "oes::addOE.  Searching for beginValid()" << endl;
     OrbElemMap::iterator it = oem.find(eph->beginValid);
     if(it!=oem.end())
     {
        const OrbElemBase* oe = it->second;
          // Found duplicate already in table
        //if(oe->ctToe==eph->ctToe)
        if(oe->isSameData(eph))
        {
            if (dbg) cout << "oes::addOE.  isSameData() returned true." << endl;
            return (false);
        }
          // Found matching beginValid but different Toe - This shouldn't happen
        else
        {
           string str = "Unexpectedly found matching beginValid times";
           stringstream os;
           os << eph->satID;
           str += " but different Toe.   SV = " + os.str();
           str += ", beginValid= " + printTime(eph->beginValid,ts);
           str += ", Toe(map)= " + printTime(eph->ctToe,ts);
           str += ", Toe(candidate)= "+ printTime(oe->ctToe," %6.0g");
           str += ". ";
           InvalidParameter exc( str );
           GPSTK_THROW(exc); 
        }
     }
        // Did not already find match to
        // beginValid in map
        // N.B:: lower_bound will return element beyond key since there is no match
     if (dbg) cout << "oes::addOE.  Did not find beginValid in map." << endl;
     it = oem.lower_bound(eph->beginValid);
        // Case where candidate is before beginning of map
     if(it==oem.begin())
     {
        if (dbg) cout << "oes::addOE.  Case of beginning of map" << endl;
        const OrbElemBase* oe = it->second;
        //if(oe->ctToe==eph->ctToe)
        if(oe->isSameData(eph))
        {
           if (dbg) cout << "oes::addOE.  isSameData() returned true." << endl;
           oem.erase(it);
           oem[eph->beginValid] = eph->clone();
           updateInitialFinal(eph);
           return (true);
        }
        if (dbg) cout << "oes::addOE.  Added to beginning of map." << endl;
        oem[eph->beginValid] = eph->clone();
        updateInitialFinal(eph);
        return (true);
     }
          // Case where candidate is after end of current map
     if(it==oem.end())
     {
        if (dbg) cout << "oes::addOE.  Candidate after end of map." << endl;
          // Get last item in map and check Toe
        OrbElemMap::reverse_iterator rit = oem.rbegin();
        const OrbElemBase* oe = rit->second;
        //if(oe->ctToe!=eph->ctToe)
        if(!oe->isSameData(eph))
        {
           if (dbg) cout << "oes::addOE.  isSameData() returned false.  Adding to end." << endl;
           oem[eph->beginValid] = eph->clone();
           updateInitialFinal(eph);
           return (true);
        }
        if (dbg) cout << "oes::addOE. isSameData() returned true.  No need to add." << endl;
        return (false);
     }
        // case where candidate is "In the middle"
        // Check if iterator points to late transmission of
        // same OrbElem as candidate
     const OrbElemBase* oe = it->second;
     //if(oe->ctToe==eph->ctToe)
     if(oe->isSameData(eph))
     {
        oem.erase(it);
        oem[eph->beginValid] = eph->clone();
        updateInitialFinal(eph);
        return (true);
     }
        // Two cases:
        //    (a.) Candidate is late transmit copy of
        //         previous OrbElemBase in table - discard (do nothing)
        //    (b.) Candidate OrbElemBase is not in table
 
        // Already checked for it==oem.beginValid() earlier
     it--;
     const OrbElemBase* oe2 = it->second;
     //if(oe2->ctToe!=eph->ctToe)
     if(!oe2->isSameData(eph))
     {
        oem[eph->beginValid] = eph->clone();
        updateInitialFinal(eph);
        return (true);
     }
     return (false);
    
   }
   catch(Exception& e)
   {
      GPSTK_RETHROW(e)
   }
 }
    
//-----------------------------------------------------------------------------

   void OrbElemStore::edit(const CommonTime& tmin, const CommonTime& tmax)
      throw()
   {
      for(UBEMap::iterator i = ube.begin(); i != ube.end(); i++)
      {
         OrbElemMap& eMap = i->second;

         OrbElemMap::iterator lower = eMap.lower_bound(tmin);
         if (lower != eMap.begin())
         { 
            for (OrbElemMap::iterator emi = eMap.begin(); emi != lower; emi++)
               delete emi->second;        
            eMap.erase(eMap.begin(), lower);
         } 

         OrbElemMap::iterator upper = eMap.upper_bound(tmax);
         if (upper != eMap.end())
         {
            for (OrbElemMap::iterator emi = upper; emi != eMap.end(); emi++)
               delete emi->second; 
            eMap.erase(upper, eMap.end());          
         }
      }

      initialTime = tmin;
      finalTime   = tmax;
   }

//-----------------------------------------------------------------------------

   unsigned OrbElemStore::size() const
      throw()
   {
      unsigned counter = 0;
      for(UBEMap::const_iterator i = ube.begin(); i != ube.end(); i++)
         counter += i->second.size();
      return counter;
   }


//-----------------------------------------------------------------------------
   bool OrbElemStore::isPresent(const SatID& id) const 
      throw()
   {
      UBEMap::const_iterator ci = ube.find(id);
      if (ci==ube.end()) return false;
      return true;
   }
   

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Goal is to find the set of orbital elements that would have been
// used by a receiver in real-time.   That is to say, the most recently
// broadcast elements (assuming the receiver has visibility to the SV
// in question).
//-----------------------------------------------------------------------------

   const OrbElemBase*
   OrbElemStore::findOrbElem(const SatID& sat, const CommonTime& t) const
      throw( InvalidRequest )
   {
          // Check to see that there exists a map of orbital elements
	  // relevant to this SV.
      UBEMap::const_iterator prn_i = ube.find(sat);
      if (prn_i == ube.end())
      {
         InvalidRequest e("No orbital elements for satellite " + asString(sat));
         GPSTK_THROW(e);
      }

         // Define reference to the relevant map of orbital elements
      const OrbElemMap& em = prn_i->second;

         // The map is ordered by beginning times of validity, which
	 // is another way of saying "earliest transmit time".  A call
         // to em.lower_bound( t ) will return the element of the map
	 // with a key "one beyond the key" assuming the t is NOT a direct
	 // match for any key. 
	 
	 // First, check for the "direct match" case
      OrbElemMap::const_iterator it = em.find(t);
         // If that fails, then use lower_bound( )
      if (it == em.end( ))    
      {
	 it = em.lower_bound(t); 
	              
	 // Tricky case here.  If the key is beyond the last key in the table,
	 // lower_bound( ) will return em.end( ).  However, this doesn't entirely
	 // settle the matter.  It is theoretically possible that the final 
	 // item in the table may have an effectivity that "stretches" far enough
	 // to cover time t.   Therefore, if it==em.end( ) we need to check 
	 // the period of validity of the final element in the table against 
	 // time t.
	 //
	 if (it==em.end())	
         {
            OrbElemMap::const_reverse_iterator rit = em.rbegin();
            if (rit->second->isValid(t)) return(rit->second);   // Last element in map works

	       // We reached the end of the map, checked the end of the map,
	       // and we still have nothing.  
            string mess = "All orbital elements found for satellite " + asString(sat) + " are too early for time "
               + (static_cast<CivilTime>(t)).printf("%02m/%02d/%04Y %02H:%02M:%02S %P");
            InvalidRequest e(mess);
            GPSTK_THROW(e);
         }
      } 

         // If the algorithm found a direct match, then we should 
	 // probably use the PRIOR set since it takes ~30 seconds
	 // from beginning of transmission to complete reception.
	 // If lower_bound( ) was called, it points to the element
	 // after the time of the key.
	 // So either way, it points ONE BEYOND the element we want.
	 //
	 // The exception is if it is pointing to em.begin( ).  If that is the case,
	 // then all of the elements in the map are too late.
      if (it==em.begin())
      {
         string mess = "All orbital elements found for satellite " + asString(sat) + " are too late for time "
            + (static_cast<CivilTime>(t)).printf("%02m/%02d/%04Y %02H:%02M:%02S %P");
         InvalidRequest e(mess);
         GPSTK_THROW(e);
      }

	 // The iterator should be a valid iterator and set one beyond
	 // the item of interest.  However, there may be gaps in the 
	 // middle of the map and cases where periods of effectivity do
	 // not overlap.  That's OK, the key represents the EARLIEST 
	 // time the elements should be used.   Therefore, we can 
	 // decrement the counter and test to see if the element is
	 // valid. 
      it--; 
      if (!(it->second->isValid(t)))
      {
	    // If we reach this throw, the cause is a "hole" in the middle of a map. 
         string mess = "No orbital elements found for satellite " + asString(sat) + " at "
            + (static_cast<CivilTime>(t)).printf("%02m/%02d/%04Y %02H:%02M:%02S %P");
         InvalidRequest e(mess);
         GPSTK_THROW(e);
      }
      return(it->second);
   } 

 
//-----------------------------------------------------------------------------
 
     const OrbElemBase*
   OrbElemStore::findNearOrbElem(const SatID& sat, const CommonTime& t) const
      throw(InvalidRequest)
   {
        // Check for any OrbElem for this SV            
      UBEMap::const_iterator prn_i = ube.find(sat);
      if (prn_i == ube.end())
      {
         InvalidRequest e("No OrbElem for satellite " + asString(sat));
         GPSTK_THROW(e);
      }
   
         // FIRST, try to find the elements that were
         // actually being broadcast at the time of 
         // interest.  That will ALWAYS be the most
         // correct response.   IF YOU REALLY THINK
         // OTHERWISE CALL ME AND LET'S TALK ABOUT 
         // IT - Brent Renfro
      try
      {
         const OrbElemBase* oep = findOrbElem(sat, t);
         return(oep);
      }      
         // No OrbElemBase in store for requested sat time  
      catch(InvalidRequest)
      {
           // Create references to map for this satellite
         const OrbElemMap& em = prn_i->second;
           /*
              Three Cases: 
                1. t is within a gap within the store
                2. t is before all OrbElemBase in the store
                3. t is after all OrbElemBase in the store
           */

           // Attempt to find next in store after t
         OrbElemMap::const_iterator itNext = em.lower_bound(t);
           // Test for case 2
         if(itNext==em.begin())
         {
            return(itNext->second);
         }
           // Test for case 3
         if(itNext==em.end())
         {
            OrbElemMap::const_reverse_iterator rit = em.rbegin();
            return(rit->second);
         }
           // Handle case 1
           // Know that itNext is not the beginning, so safe to decrement
         CommonTime nextBeginValid = itNext->first;
         OrbElemMap::const_iterator itPrior = itNext;
         itPrior--;
         CommonTime lastEndValid = itPrior->second->endValid;
         double diffToNext = nextBeginValid-t;
         double diffFromLast = t - lastEndValid;
         if(diffToNext>diffFromLast)
         {
            return(itPrior->second);
         }
         return(itNext->second);
      }
   }
      
//-----------------------------------------------------------------------------
   const OrbElemBase* OrbElemStore::
   findToe(const SatID& sat, const CommonTime& t)
      const throw(InvalidRequest)
   {
         // If the TimeSystem of the requested t doesn't match
         // the TimeSystem stored in this store, throw an error.
      if (timeSysForStore!=t.getTimeSystem())
      {
         std::stringstream ss;
         ss << "Mismatched TimeSystems.  Time system of store: ";
         ss << timeSysForStore << ", Time system of argument: ";
         ss << t.getTimeSystem();
         InvalidRequest e(ss.str());
         GPSTK_THROW(e);
      }
   
         // Check for any OrbElem for this SV            
      UBEMap::const_iterator prn_i = ube.find(sat);
      if (prn_i == ube.end())
      {
         InvalidRequest e("No OrbElem for satellite " + asString(sat));
         GPSTK_THROW(e);
      }

         // Create a reference to map for this satellite
      const OrbElemMap& em = prn_i->second;

         // We are looking for an exact match for a Toe.
         // The map is keyed with the beginValid time, so the
         // only way to determine if there is a match is to iterate
         // over the map and check.
      OrbElemMap::const_iterator cit;
      for (cit=em.begin();cit!=em.end();cit++)
      {
         const OrbElemBase* candidate = cit->second;
         if (candidate->ctToe==t) return(candidate);
      }         

         // If we reached this point, we didn't find a match.
      std::stringstream ss;
      ss << "No match found for SV " << sat;
      ss << " with Toe " << printTime(t,"%02m/%02d/%04Y %02H:%02M:%02S");
      InvalidRequest e(ss.str());
      GPSTK_THROW(e);

         // Keep the compiler happy.
      OrbElemBase* dummy = 0;    
      return(dummy); 
   }


//-----------------------------------------------------------------------------

   int OrbElemStore::addToList(std::list<OrbElemBase*>& v) const
      throw()
   {
      int n = 0;
      UBEMap::const_iterator prn_i;
      for (prn_i = ube.begin(); prn_i != ube.end(); prn_i++)
      {
         const OrbElemMap& em = prn_i->second;
         OrbElemMap::const_iterator ei;
         for (ei = em.begin(); ei != em.end(); ei++)
         {
            v.push_back(ei->second->clone());
            n++;
         }
      }
      return n;
   } 

//-----------------------------------------------------------------------------

   /// Remove all data from this collection.
   void OrbElemStore::clear()
         throw()
   {
      for( UBEMap::iterator ui = ube.begin(); ui != ube.end(); ui++)
      {
         OrbElemMap& oem = ui->second;
          for (OrbElemMap::iterator oi = oem.begin(); oi != oem.end(); oi++)
         {
            delete oi->second;
         }
      } 
     ube.clear();
     initialTime = gpstk::CommonTime::END_OF_TIME;
     finalTime = gpstk::CommonTime::BEGINNING_OF_TIME;
     initialTime.setTimeSystem(timeSysForStore);
     finalTime.setTimeSystem(timeSysForStore); 
   }

//-----------------------------------------------------------------------------

   const OrbElemStore::OrbElemMap&
   OrbElemStore::getOrbElemMap( const SatID& sat ) const
      throw( InvalidRequest )
   {
      validSatSystem(sat);

      UBEMap::const_iterator prn_i = ube.find(sat);
      if (prn_i == ube.end())
      {
         InvalidRequest e("No OrbElemBase for satellite " + asString(sat));
         GPSTK_THROW(e);
      }
      return(prn_i->second);
   }
   
//-----------------------------------------------------------------------------

   list<gpstk::SatID> OrbElemStore::getSatIDList() const
   {
      list<gpstk::SatID> retList;
      for( UBEMap::const_iterator ui = ube.begin(); ui != ube.end(); ui++)
      {
         SatID sid = ui->first;
         retList.push_back(sid);
      } 
      return retList;
   }


//-----------------------------------------------------------------------------

   bool OrbElemStore::isSatSysPresent(const SatID::SatelliteSystem ss) const
   {
      list<SatID::SatelliteSystem>::const_iterator cit;
      for (cit=sysList.begin();cit!=sysList.end();cit++)
      {
         SatID::SatelliteSystem ssTest = *cit;
         if (ssTest==ss) return true;
      }
      return false;
   }
   
   void OrbElemStore::addSatSys(const SatID::SatelliteSystem ss)
   {
      sysList.push_back(ss);
   }
   
   
} // namespace
