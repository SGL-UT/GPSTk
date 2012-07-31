#pragma ident "$Id: GPSOrbElemStore.cpp 3178 2012-06-29 16:32:18Z snelsen $"

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
 * @file GPSOrbElemStore.cpp
 * Store GPS broadcast OrbElem information, and access by satellite and time
 */

#include <iostream>
#include <fstream>
#include <iomanip>

#include "StringUtils.hpp"
#include "GPSOrbElemStore.hpp"
#include "MathBase.hpp"
#include "CivilTime.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;
using gpstk::StringUtils::asString;


namespace gpstk
{

//--------------------------------------------------------------------------

   Xvt GPSOrbElemStore::getXvt(const SatID& sat, const CommonTime& t) const
      throw( InvalidRequest )
   {
      OrbElem* ref;
      return getXvt(sat, t, ref);
   } 

//--------------------------------------------------------------------------

   Xvt GPSOrbElemStore::getXvt(const SatID& sat, const CommonTime& t, OrbElem* ref) const
      throw( InvalidRequest )
   {
      try
      {
         // test for GPS satellite system in sat?
         const OrbElem* eph = findOrbElem(sat,t);
         ref = eph->clone();
         Xvt sv = eph->svXvt(t);
         return sv;
      }
      catch(InvalidRequest& ir)
      {
         GPSTK_RETHROW(ir);
      }
   }

//--------------------------------------------------------------------------

   const OrbElem*
   GPSOrbElemStore::findOrbElem(const SatID& sat, const CommonTime& t) const
      throw( InvalidRequest )
   {
      try
      {
         validSatSystem(sat);

       //  return strictMethod ? findUserOrbElem(sat, t) : findNearOrbElem(sat, t);
           return findUserOrbElem(sat, t);
      }
      catch(InvalidRequest& ir)
      {
         GPSTK_RETHROW(ir);
      }
   }

//--------------------------------------------------------------------------

   bool GPSOrbElemStore::isHealthy(const SatID& sat, const CommonTime& t) const
      throw( InvalidRequest )
   {
      try
      {
         validSatSystem(sat);

         // test for GPS satellite system in sat?
         const OrbElem* eph = findOrbElem(sat, t);
         
         return eph->isHealthy();
      }
      catch(InvalidRequest& ir)
      {
         GPSTK_RETHROW(ir);
      }
   } // end of GPSOrbElemStore::getHealth()

//--------------------------------------------------------------------------

   void GPSOrbElemStore::dump(std::ostream& s, short detail) const
      throw()
   {
      UBEMap::const_iterator it;
      static const string fmt("%4F %10.3g = %04Y/%02m/%02d %02H:%02M:%02S %P");

      s << "Dump of GPSOrbElemStore:\n";
      if (detail==0)
      {
         s << " Span is " << (initialTime == CommonTime::END_OF_TIME
                                      ? "End_time" : printTime(initialTime,fmt))
           << " to " << (finalTime == CommonTime::BEGINNING_OF_TIME
                                      ? "Begin_time" : printTime(finalTime,fmt))
           << " with " << size() << " entries."
           << std::endl;
      } // end if-block
      else
      {
         for (it = ube.begin(); it != ube.end(); it++)
         {
            const OrbElemMap& em = it->second;
            s << "  BCE map for satellite " << it->first
              << " has " << em.size() << " entries." << std::endl;
            OrbElemMap::const_iterator ei;

            for (ei = em.begin(); ei != em.end(); ei++) 
            {
               const OrbElem* oe = ei->second;
               if (detail==1)
               {
                  s << "PRN " << setw(2) << it->first
                    << " TOE " << printTime(oe->ctToe,fmt)
                    << " TOC " << fixed << setw(10) << setprecision(3)
                    << oe->ctToe
                    << " KEY " << printTime(ei->first,fmt);
                  switch(oe->type)
                  {
                     case OrbElem::OrbElemFIC9:
                     {
                        s << " FIC9";
                        break;
                     }
                     case OrbElem::OrbElemFIC109:
                     {
                        s << " FIC109";
                        break;
                     }
                     case OrbElem::OrbElemRinex:
                     {
                        s << " Rinex";
                        break;
                     }
                     default:
                        s << " Unknown";
                  }   
                  s << std::endl;
               }
               else
                  oe->dump(s);

            } //end inner for-loop */

         } // end outer for-loop
   
         s << "  End of GPSOrbElemStore data." << std::endl << std::endl;

      } //end else-block

   } // end GPSOrbElemStore::dump

//------------------------------------------------------------------------------------
// Keeps only one OrbElem for a given SVN and Toe.
// It should keep the one with the earliest transmit time.
//------------------------------------------------------------------------------------ 
   bool GPSOrbElemStore::addOrbElem(const OrbElem& eph)
      throw()
   {
      bool rc = false;
     try
     {

     SatID sid = eph.satID;
     OrbElemMap& oem = ube[sid];
     string ts = "%02m/%02d/%02y %02H:%02M:%02S";
     int PRN = sid.id;
     int testPRN = 0;
     if(PRN==testPRN)
     {
        cout << "****Testing PRN " << testPRN << " beginValid: " << printTime(eph.beginValid, ts)
             << " Toe: " << printTime(eph.ctToe,ts) << endl;
     }
       // if map is empty, load object and return
     if (oem.size()==0)
     {
        if(PRN==testPRN)
           cout << "First object load: " << printTime(eph.ctToe,ts) << endl;
        oem[eph.beginValid] = eph.clone();
        updateInitialFinal(eph);
        return (true);
     }
       // Search for beginValid in current keys.
       // If found candidate, should be same data
       // as already in table. Test this by comparing
       // Toe values.
     if(PRN==testPRN)
     {
        cout << "Attempting to find key." << endl;
        cout << "eph.beginValid = " << eph.beginValid << endl;
     }
     OrbElemMap::iterator it = oem.find(eph.beginValid);
     if(PRN==testPRN)
        cout << "Returned from find operation." << endl;
     if(it!=oem.end())
     {
        if(PRN==testPRN)
           cout << "Found the key." << endl;
        const OrbElem* oe = it->second;
          // Found duplicate already in table
        if(oe->ctToe==eph.ctToe)
        {
            if(PRN==testPRN)
               cout << "Found duplicate: " << printTime(eph.ctToe,ts) << endl; 
            return (false);
        }
          // Found matching beginValid but different Toe - This shouldn't happen
        else
        {
           if(PRN==testPRN)
              cout << "Doing the impossible." << endl;
           cout << "PRN = " << eph.satID.id << endl;
           cout << " beginValid = " << printTime(eph.beginValid,ts) << endl;          
           cout << " Toe = " << printTime(eph.ctToe,ts) << endl;
           cout << "oe->ctToe = " << oe->ctToe << endl;
           cout << "oe->transmitTime = " << oe->beginValid << endl;   
           exit(1); 
        }
     }
     if(PRN==testPRN)
        cout << "Find failed." << endl;
        // Did not already find match to
        // beginValid in map
        // N.B:: lower_bound will reutrn element beyond key since there is no match
     it = oem.lower_bound(eph.beginValid);
        // Case where candidate is before beginning of map
     if(it==oem.begin())
     {
        const OrbElem* oe = it->second;
        if(oe->ctToe==eph.ctToe)
        {
           if(PRN==testPRN)
              cout << "Replacing beginning of map with earlier copy: " << printTime(eph.ctToe,ts) << endl;
           oem.erase(it);
           oem[eph.beginValid] = eph.clone();
           updateInitialFinal(eph);
           return (true);
        }
        if(PRN==testPRN)
          cout << "Append to beginning of map: " << printTime(eph.ctToe,ts) << endl;
        oem[eph.beginValid] = eph.clone();
        updateInitialFinal(eph);
        return (true);
     }
        
    // it = oem.lower_bound(eph.beginValid);
          // Case where candidate is after end of current map
     if(it==oem.end())
     {
          // Get last item in map and check Toe
        OrbElemMap::reverse_iterator rit = oem.rbegin();
        const OrbElem* oe = rit->second;
        if(oe->ctToe!=eph.ctToe)
        {
           if(PRN==testPRN)
              cout << "Adding to end of map." << printTime(eph.ctToe,ts) << endl;
           oem[eph.beginValid] = eph.clone();
           updateInitialFinal(eph);
           return (true);
        }
        if(PRN==testPRN)
           cout << "Found copy of existing end of map: " << printTime(eph.ctToe,ts) << endl; 
        return (false);
     }
        // case where candidate is "In the middle"
        // Check if iterator points to late transmission of
        // same OrbElem as candidate
     const OrbElem* oe = it->second;
     if(oe->ctToe==eph.ctToe)
     {
        if(PRN==testPRN)
           cout << "Replacing existing object with earlier copy: " << printTime(eph.ctToe,ts) << endl;
        oem.erase(it);
        oem[eph.beginValid] = eph.clone();
        updateInitialFinal(eph);
        return (true);
     }
        // Two cases:
        //    (a.) Candidate is late transmit copy of
        //         previous OrbElem in table - discard
        //    (b.) Candidate OrbElem is not in table
 
        // Already checked for it==oem.beginValid() earlier
     it--;
     const OrbElem* oe2 = it->second;
     if(oe2->ctToe!=eph.ctToe)
     {
        if(PRN==testPRN)
           cout << "Adding new object in middle of map: " << printTime(eph.ctToe,ts) << endl;
        oem[eph.beginValid] = eph.clone();
        updateInitialFinal(eph);
        return (true);
     }
     if(PRN==testPRN)
        cout << "Discarding later copy of existing object in middle of map: " << printTime(eph.ctToe,ts) << endl;
     return (false);
    
   }
   
   catch(Exception& e)
   {
      cout << "Caught a gpstk:exception" << endl;
      cout << e.getText() << endl;
      exit(1);
   }
   catch(...)
   {
      cout << "Caught unknown exception" << endl;
    //  cout << e.getText() << endl;
      exit(1);
   }
 }
    
//-----------------------------------------------------------------------------

   void GPSOrbElemStore::edit(const CommonTime& tmin, const CommonTime& tmax)
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

   unsigned GPSOrbElemStore::size() const
      throw()
   {
      unsigned counter = 0;
      for(UBEMap::const_iterator i = ube.begin(); i != ube.end(); i++)
         counter += i->second.size();
      return counter;
   }

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Goal is to find the set of orbital elements that would have been
// used by a receiver in real-time.   That is to say, the most recently
// broadcast elements (assuming the receiver has visibility to the SV
// in question).
//-----------------------------------------------------------------------------

   const OrbElem*
   GPSOrbElemStore::findUserOrbElem(const SatID& sat, const CommonTime& t) const
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
 
   

  /* const OrbElem*
   GPSOrbElemStore::findUserOrbElem(const SatID& sat, const CommonTime& t) const
      throw( InvalidRequest )
   {
      validSatSystem(sat);

      UBEMap::const_iterator prn_i = ube.find(sat);
      if (prn_i == ube.end())
      {
         InvalidRequest e("No OrbElem for satellite " + asString(sat));
         GPSTK_THROW(e);
      }

      const OrbElemMap& em = prn_i->second;
      CommonTime t1, t2, Tot = CommonTime::BEGINNING_OF_TIME;
      OrbElemMap::const_iterator it = em.end();

      // Find eph with (Toe-(fitint/2)) > t - 4 hours
      // Use 4 hours b/c it's the default fit interval.
      // Backup one OrbElem to make sure you get the
      // correct one in case of fit intervals greater 
      // than 4 hours.
      OrbElemMap::const_iterator ei = em.upper_bound(t - 4 * 3600); 
      if (!em.empty() && ei != em.begin() )
      {
         ei--;
      }
      
      for (; ei != em.end(); ei++)
      {
         const OrbElem& current = ei->second;
         // t1 = beginning of validity
         t1 = ei->first;
         // t2 = HOW time
         t2 = current.beginValid;

         // Ephemerides are ordered by fit interval.  
         // If the start of the fit interval is in the future, 
         // this and any more ephemerides are not the one you are
         // looking for.
         if( t1 > t ) 
         {
            break;
         }
         
         double dt1 = t - t1;
         double dt2 = t - t2;
         double fitDuration = current.endValid - current.beginValid;
//cout << "time t " << (static_cast<CivilTime>(t)).printf("%02m/%02d/%04Y %02H:%02M:%02S") << " // ";
//cout << "time t1 " << (static_cast<CivilTime>(t1)).printf("%02m/%02d/%04Y %02H:%02M:%02S") << " // ";
//cout << "time t2 " << (static_cast<CivilTime>(t2)).printf("%02m/%02d/%04Y %02H:%02M:%02S") << " // ";
//cout << "dt1 " << fixed << setprecision(3) << dt1 << " and dt2 " << dt2 << endl;
         if (dt1 >= 0 &&                           // t is after start of fit interval
             dt1 < fitDuration &&  // t is within the fit interval
             dt2 >= 0 &&                           // t is after Tot
             t2 > Tot )                            // this eph has the latest Tot
         {
            it = ei;
            Tot = t2;
         }
      }

      if (it == em.end())
      {
         string mess = "No eph found for satellite " + asString(sat) + " at "
            + (static_cast<CivilTime>(t)).printf("%02m/%02d/%04Y %02H:%02M:%02S %P");
         InvalidRequest e(mess);
         GPSTK_THROW(e);
      }

      return it->second;
   } */

//-----------------------------------------------------------------------------

 /*  const OrbElem*
   GPSOrbElemStore::findNearOrbElem(const SatID& sat, const CommonTime& t) const
      throw( InvalidRequest )
   {
      validSatSystem(sat);

      UBEMap::const_iterator prn_i = ube.find(sat);
      if (prn_i == ube.end())
      {
         InvalidRequest e("No OrbElem for satellite " + asString(sat));
         GPSTK_THROW(e);
      }

      const OrbElemMap& em = prn_i->second;
      double dt2min = -1;
      CommonTime tstart, how;
      OrbElemMap::const_iterator it = em.end();

      // Find eph with (Toe-(fitint/2)) > t - 4 hours
      // Use 4 hours b/c it's the default fit interval.
      // Backup one OrbElem to make sure you get the
      // correct one in case of fit intervals greater 
      // than 4 hours.
      OrbElemMap::const_iterator ei = em.upper_bound(t - 4 * 3600); 
      if (!em.empty() && ei != em.begin() )
      {
         ei--;
      }
      
      for (; ei != em.end(); ei++)
      {
         const OrbElem& current = ei->second;
         // tstart = beginning of validity
         tstart = ei->first;
         // how = HOW time
         how = current.beginValid;

         // Ephemerides are ordered by time of start of fit interval.  
         // If the start of the fit interval is in the future, 
         // this and any more ephemerides are not the one you are
         // looking for.
         if( tstart > t ) break;
         
         double dt1 = t - tstart;
         double dt2 = t - how;
         double fitDuration = current.endValid - current.beginValid;
         if (dt1 >= 0 &&                           // t is after start of fit interval
             dt1 <= fitDuration &&  // t is within the fit interval
             (dt2min == -1 || fabs(dt2) < dt2min))  // t is closest to HOW
         {
            it = ei;
            dt2min = fabs(dt2);
         }
      }

      if (it == em.end())
      {
         string mess = "No eph found for satellite " + asString(sat) + " at "
            + (static_cast<CivilTime>(t)).printf("%02m/%02d/%04Y %02H:%02M:%02S %P");
         InvalidRequest e(mess);
         GPSTK_THROW(e);
      }
      return it->second;
   } */

//-----------------------------------------------------------------------------

   int GPSOrbElemStore::addToList(std::list<OrbElem*>& v) const
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

   const GPSOrbElemStore::OrbElemMap&
   GPSOrbElemStore::getOrbElemMap( const SatID& sat ) const
      throw( InvalidRequest )
   {
      validSatSystem(sat);

      UBEMap::const_iterator prn_i = ube.find(sat);
      if (prn_i == ube.end())
      {
         InvalidRequest e("No OrbElem for satellite " + asString(sat));
         GPSTK_THROW(e);
      }
      return(prn_i->second);
   }
   
} // namespace
