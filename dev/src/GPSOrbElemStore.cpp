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
      OrbElem ref;
      return getXvt(sat, t, ref);
   }

//--------------------------------------------------------------------------

   Xvt GPSOrbElemStore::getXvt(const SatID& sat, const CommonTime& t, OrbElem& ref) const
      throw( InvalidRequest )
   {
      try
      {
         // test for GPS satellite system in sat?
         const OrbElem& eph = findOrbElem(sat,t);
         ref = eph;
         Xvt sv = eph.svXvt(t);
         return sv;
      }
      catch(InvalidRequest& ir)
      {
         GPSTK_RETHROW(ir);
      }
   }

//--------------------------------------------------------------------------

   const OrbElem&
   GPSOrbElemStore::findOrbElem(const SatID& sat, const CommonTime& t) const
      throw( InvalidRequest )
   {
      try
      {
         validSatSystem(sat);

         return strictMethod ? findUserOrbElem(sat, t) : findNearOrbElem(sat, t);
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
         const OrbElem& eph = findOrbElem(sat, t);
         
         return eph.isHealthy();
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
           << " with " << ubeSize() << " entries."
           << std::endl;
      } // end if-block
      else
      {
         for (it = ube.begin(); it != ube.end(); it++)
         {
            const OrbElemMap& em = it->second;
          //  dumpOnePRN(s,em);
            s << "  BCE map for satellite " << it->first
              << " has " << em.size() << " entries." << std::endl;
            OrbElemMap::const_iterator ei;

            for (ei=em.begin(); ei != em.end(); ei++) {
               if (detail==1)
               {
                  s << "PRN " << setw(2) << it->first
                    << " TOE " << printTime(ei->second.ctToe,fmt)
                    << " TOC " << fixed << setw(10) << setprecision(3)
                    << ei->second.ctToe
                    << " KEY " << printTime(ei->first,fmt);
                  switch(ei->second.type)
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
                  ei->second.dump(s);

            } //end inner for-loop */

         } // end outer for-loop
   
         s << "  End of GPSOrbElemStore data." << std::endl << std::endl;

      } //end else-block

   } // end GPSOrbElemStore::dump

//--------------------------------------------------------------------------
/*
   void GPSOrbElemStore::dumpOnePRN(std::ostream& s, OrbElemMap& em) const
      throw()
   {
        s << "  BCE map for satellite " << it->first
              << " has " << em.size() << " entries." << std::endl;
            OrbElemMap::const_iterator ei;

            for (ei=em.begin(); ei != em.end(); ei++) {
               if (detail==1)
               {
                  s << "PRN " << setw(2) << it->first
                    << " TOE " << printTime(ei->second.getEphEpoch(),fmt)
                    << " TOC " << fixed << setw(10) << setprecision(3)
                    << ei->second.getEphEpoch()
                    << " KEY " << printTime(ei->first,fmt);
                  switch(ei->second.type)
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
                  ei->second.dump(s);

            } //end inner for-loop
   } */

//--------------------------------------------------------------------------
// Keeps only one OrbElem with a given IODC/time.
// It should keep the one with the earliest transmit time.
//--------------------------------------------------------------------------

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
        oem[eph.beginValid] = eph;
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
        //dumpOnePRN(cout, oem);
     }
     OrbElemMap::iterator it = oem.find(eph.beginValid);
     if(PRN==testPRN)
        cout << "Returned from find operation." << endl;
     if(it!=oem.end())
     {
        if(PRN==testPRN)
           cout << "Found the key." << endl;
        OrbElem& oe = it->second;
          // Found duplicate already in table
        if(oe.ctToe==eph.ctToe)
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
        OrbElem& oe = it->second;
        if(oe.ctToe==eph.ctToe)
        {
           if(PRN==testPRN)
              cout << "Replacing beginning of map with earlier copy: " << printTime(eph.ctToe,ts) << endl;
           eraseAndAdd(oem,it,eph);
           return (true);
        }
        if(PRN==testPRN)
          cout << "Append to beginning of map: " << printTime(eph.ctToe,ts) << endl;
        oem[eph.beginValid] = eph;
        updateInitialFinal(eph);
        return (true);
     }
        
    // it = oem.lower_bound(eph.beginValid);
          // Case where candidate is after end of current map
     if(it==oem.end())
     {
          // Get last item in map and check Toe
        OrbElemMap::reverse_iterator rit = oem.rbegin();
        OrbElem& oe = rit->second;
        if(oe.ctToe!=eph.ctToe)
        {
           if(PRN==testPRN)
              cout << "Adding to end of map." << printTime(eph.ctToe,ts) << endl;
           oem[eph.beginValid] = eph;
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
     if(it->second.ctToe==eph.ctToe)
     {
        if(PRN==testPRN)
           cout << "Replacing existing object with earlier copy: " << printTime(eph.ctToe,ts) << endl;
        eraseAndAdd(oem,it,eph);
        return (true);
     }
        // Two cases:
        //    (a.) Candidate is late transmit copy of
        //         previous OrbElem in table - discard
        //    (b.) Candidate OrbElem is not in table
 
        // Already checked for it==oem.beginValid() earlier
     it--;
     if(it->second.ctToe!=eph.ctToe)
     {
        if(PRN==testPRN)
           cout << "Adding new object in middle of map: " << printTime(eph.ctToe,ts) << endl;
        oem[eph.beginValid] = eph;
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
    /* else
     {
        bool done = false;
        bool matchingToeFound = false;
        OrbElemMap::iterator it=oem.begin();
        OrbElem& begTest = it->second;
        if(eph.ctToe<begTest.ctToe) done = true;       
        CommonTime lower_test = eph.ctToe - ( SEC_PER_DAY / 2 );
        it = oem.lower_bound(lower_test);
        if(it != oem.begin()) it--;
        OrbElemMap::iterator itEnd = oem.upper_bound(eph.ctToe); 
          
           // Scan OrbElemMap for this SV from beginning
        if(eph.satID.id==29)
        {
          
        if(itEnd==oem.end())
            cout << "itEnd = end" << endl;
        cout << "eph.ctToe = " << eph.ctToe << endl;
        cout << "sid.id = " << sid.id << endl;
        OrbElem& beg = it->second;
        cout << "beg Toe = " << beg.ctToe << endl;
        if(itEnd!=oem.end())
        {
           OrbElem& end = itEnd->second;
           cout << "end Toe = " << end.ctToe << endl;
        }
        }
        while (!done)
        {
           OrbElem& oe = it->second;

              // If the two epoch times are identical, then
              // these are actually the same transmitted data set.
           if (oe.ctToe==eph.ctToe)
           {
              matchingToeFound = true; 
              break;
           }
           it++;
           if (it==itEnd) done = true;
        }
        if(eph.satID.id==29 && matchingToeFound) cout << "matchingToeFound = true" << endl;
        else cout << "matchingToeFound = false" << endl;
          
        if(matchingToeFound)
        {  
                 // Two copies of same transmitted data set.  We want to keep
                 // the earliest transmitted.  That means we only
                 // have to take action if the new candidate is
                 // earlier than the object already in the map. 
           OrbElem& oe = it->second;
           if (eph.beginValid < oe.beginValid)
           {
              cout << "Matching Toe found with replacement." << endl;
              cout << "PRN: " << sid.id << endl;
              cout << "Existing beginValid = " << oe.beginValid << endl;
              cout << "Candidate beginValid = " << eph.beginValid << endl;
              oem.erase(it);
              oem[eph.beginValid] = eph;
              rc = true;
           }                     
        }

           // If the two times are NOT identical, we need to
           // verify there is NOT already an OrbElem with 
           // the desired key.
        if (!matchingToeFound)     
        {
           it = oem.find(eph.beginValid);
           if (it==oem.end())
           {
              oem[eph.beginValid] = eph;
              rc = true; 
           }
           else
           {
              rc = false;
           }
        }      
     }
     

      // In any case, update the initial and final times

    
   
      if (rc)
      {
        if (eph.beginValid<initialTime)       
          initialTime = eph.beginValid;
         
        if (eph.endValid>finalTime)               
          finalTime = eph.endValid;
        
      }
    
      return rc;
   
   }  */

//-----------------------------------------------------------------------------

   void GPSOrbElemStore::edit(const CommonTime& tmin, const CommonTime& tmax)
      throw()
   {
      for(UBEMap::iterator i = ube.begin(); i != ube.end(); i++)
      {
         OrbElemMap& eMap = i->second;

         OrbElemMap::iterator lower = eMap.lower_bound(tmin);
         if (lower != eMap.begin())
            eMap.erase(eMap.begin(), lower);

         OrbElemMap::iterator upper = eMap.upper_bound(tmax);
         if (upper != eMap.end())
            eMap.erase(upper, eMap.end());
      }

      initialTime = tmin;
      finalTime   = tmax;
   }

//-----------------------------------------------------------------------------

   unsigned GPSOrbElemStore::ubeSize() const
      throw()
   {
      unsigned counter = 0;
      for(UBEMap::const_iterator i = ube.begin(); i != ube.end(); i++)
         counter += i->second.size();
      return counter;
   }

//-----------------------------------------------------------------------------

   const OrbElem&
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
   }

//-----------------------------------------------------------------------------

   const OrbElem&
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
   }

//-----------------------------------------------------------------------------

   int GPSOrbElemStore::addToList(std::list<OrbElem>& v) const
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
            v.push_back(ei->second);
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
