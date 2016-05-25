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
 * @file OrbSysStore.cpp
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <set>

#include "StringUtils.hpp"
#include "OrbSysStore.hpp"

#include "CivilTime.hpp"
#include "TimeString.hpp"

using namespace std;
using gpstk::StringUtils::asString;


namespace gpstk
{

//------------------------------------------------------------------------------
   bool OrbSysStore::addMessage(const OrbDataSys* p)
         throw(InvalidRequest,Exception)
   {
      if (debugLevel) cout << "Entering addMessage(OrbDataSys*)" << endl;
   
         // Set up the indexing information for convenience
      const CommonTime& ct = p->beginValid;
      const unsigned long UID = p->UID;
      const ObsID& oidr = p->obsID;
      const SatID& sidr = p->satID;
      NavID navtype = NavID(sidr,oidr);

         // See if there is already a message in the store that 
         // matches this one.  If not, it needs to be added.
         // If so, there are several cases.  
         //   - If it matches and has the same time, simply return.
         //   - If it matches, but has a different time,...
         //     -- If storeAll is set, add it.
         //     -- If !storeAll and time of new object is LATER than 
         //        time of existing object, return without adding
         //     -- If !storeAll and time of new object is earlier
         //        than time of existing object, delete existing 
         //        object and add this object.
      bool addItem = false;   // default case 
      try
      {
         const OrbDataSys* ptest = find(sidr,navtype,UID,ct);
       
         if (!ptest->isSameData(p)) addItem = true;

            // Handle the "same data" cases.
         else
         {
            if (ptest->beginValid==ct) return false;
            if (storeAll) addItem = true;
            else
            {
               if (ct > ptest->beginValid) return false;
               addItem = true; 
               deleteMessage(sidr,navtype,UID,ct);               
            }            
         }
      }
         // If the find threw an exception, that DEFINITELY means
         // this data is not currently loaded. 
      catch (InvalidRequest ir)
      {
         addItem = true;
      }

      if (addItem)
      {
         NM_UID_MSG_MAP& mapr1 = msgMap[sidr];
         UID_MSG_MAP& mapr2 = mapr1[navtype];
         MSG_MAP& mapr3 = mapr2[UID];
         MSG_MAP::value_type newobj(ct,p->clone());
         mapr3.insert(newobj); 
         
         updateInitialTime(p);               
      }

      return true; 
   }

//--------------------------------------------------------------------------
//  Locate the item in the map matching the provided
//  parameters and delete it
   void OrbSysStore::deleteMessage(const SatID& sat, 
                             const NavID& navtype,
                             const unsigned long UID, 
                             const CommonTime& t)
   {
      SAT_NM_UID_MSG_MAP::iterator it1;
      NM_UID_MSG_MAP::iterator it2;
      UID_MSG_MAP::iterator it3;
      MSG_MAP::iterator it4;

         // First step is to establish if there are any messages
         // in the store matching the requested satellite, nav message 
         // type and unique ID. If any of these fail, simply return.
      stringstream failString;
      it1 = msgMap.find(sat);
      if (it1==msgMap.end()) return;

      NM_UID_MSG_MAP& NMmapr = it1->second;
      it2 = NMmapr.find(navtype);
      if (it2==NMmapr.end()) return;

      UID_MSG_MAP& UIDmapr = it2->second;
      it3 = UIDmapr.find(UID);
      if (it3==UIDmapr.end()) return;

         // Found a map of candidate messages.
      MSG_MAP& mapr = it3->second;

      it4 = mapr.find(t);
      if (it4==mapr.end()) return;

      mapr.erase(it4);
   }

//--------------------------------------------------------------------------
   void OrbSysStore::dump(std::ostream& s, short detail) const
      throw()
   {
      if (debugLevel) cout << "Entering dump()" << endl;

      if (detail==1)
      {
         dumpTerse(s);
         return;
      }

      if (detail==2)
      {
         dumpContents(s);
         return;
      }

      if (detail==3)
      {
         dumpTerseTimeOrder(s);
         return;
      }


         // If detail==0 (or at least !=1 and !=2) generate a summary
         // table of the contents of the store.
      s << "**********************************************************" << endl;
      s << " Summary Table of OrbSysStore" << endl;
      s << endl;
      
         // Create a list of the NavIDs found in this set of maps.
      set<NavID> navSet;
      set<NavID>::const_iterator cit;
      SAT_NM_UID_MSG_MAP::const_iterator cit1;
      NM_UID_MSG_MAP::const_iterator cit2;
      UID_MSG_MAP::const_iterator cit3;
      MSG_MAP::const_iterator cit4;
      for (cit1=msgMap.begin();cit1!=msgMap.end();cit1++)
      {
         const NM_UID_MSG_MAP& NMmapr = cit1->second;
         for (cit2=NMmapr.begin();cit2!=NMmapr.end();cit2++)
         {
            const NavID& navType = cit2->first;
            cit = navSet.find(navType);
            if (cit==navSet.end()) navSet.insert(navType);
         }
      }      

      list<SatID> satIDList = getSatIDList();
      list<SatID>::const_iterator csat; 
      typedef map<unsigned short, unsigned long> SUB_MAP;
      
         // For each NavID, build a map<CommonTime, map<SatID.id, UID>>
         // for all the unique messages received.   HEAVEN HELP the user who
         // calls dump( ) for a storeAll map. 
         // Then unspool the multimap to the output stream 
      for (cit=navSet.begin();cit!=navSet.end();cit++)
      {
         bool foundAtLeastOneEntry = false;
         const NavID& navTypeTarget = *cit; 
         map<CommonTime, SUB_MAP> tempMap;
         for (cit1=msgMap.begin();cit1!=msgMap.end();cit1++)
         {
            const SatID& sidr = cit1->first;
            const NM_UID_MSG_MAP& NMmapr = cit1->second;
            for (cit2=NMmapr.begin();cit2!=NMmapr.end();cit2++)
            {
               const NavID& navType = cit2->first;

                  // If this is not the type of nav we are interested in
                  // skip it. 
               if (navType!=navTypeTarget) continue;

                  // Otherwise, iterate over the submaps
               const UID_MSG_MAP& UIDmapr = cit2->second;
               for (cit3=UIDmapr.begin();cit3!=UIDmapr.end();cit3++)
               {
                  const unsigned long UID = cit3->first;
                  const MSG_MAP& mapr = cit3->second;
                  for (cit4=mapr.begin();cit4!=mapr.end();cit4++)
                  {
                     const CommonTime& ctr = cit4->first;
                     SUB_MAP& subMap = tempMap[ctr];
                     SUB_MAP::value_type inp(sidr.id,UID);
                     subMap.insert(inp); 
                     foundAtLeastOneEntry = true;
                  }
               }
            }
         } 

            // Only output the table if there is at least one entry
         if (!foundAtLeastOneEntry) continue;

            // Output header
         s << "HH:MM:SS";
         for (csat=satIDList.begin();csat!=satIDList.end();csat++)
         {
            const SatID& sidr = *csat;
            unsigned short testID = sidr.id;
            s << " " << setw(3) << testID; 
         }
         s << endl;

            // Output body of table
         map<CommonTime, SUB_MAP>::const_iterator t1;
         SUB_MAP::const_iterator t2;
         s << setfill(' ');
         for (t1=tempMap.begin();t1!=tempMap.end();t1++)
         {
            const CommonTime& ctr = t1->first;
            s << printTime(ctr,"%02H:%02M:%02S");
            const SUB_MAP& sMap = t1->second;
            for (csat=satIDList.begin();csat!=satIDList.end();csat++)
            {
               const SatID& sidr = *csat;
               t2 = sMap.find(sidr.id);
               if (t2==sMap.end()) s << "   -";
               else 
               {
                  unsigned long uid = t2->second;
                  s << " " << setw(3) << uid;
               }
            }
            s << endl;
         }
      } 
         
   } // end OrbSysStore::dump

//-----------------------------------------------------------------------------
   void OrbSysStore::dumpTerse(std::ostream& s)
         const throw()
   {
      s << "**********************************************************" << endl;
      s << " One-line summary of non-orbit constellation overhead data" << endl;
      s << "       Sat  ID mm/dd/yyyy HH:MM:SS  Data" << endl;
      SAT_NM_UID_MSG_MAP::const_iterator cit1;
      NM_UID_MSG_MAP::const_iterator cit2;
      UID_MSG_MAP::const_iterator cit3;
      MSG_MAP::const_iterator cit4; 
      for (cit1=msgMap.begin();cit1!=msgMap.end();cit1++)
      {
         const NM_UID_MSG_MAP& NMmapr = cit1->second;
         for (cit2=NMmapr.begin();cit2!=NMmapr.end();cit2++)
         {
            const UID_MSG_MAP& UIDmapr = cit2->second;
            for (cit3=UIDmapr.begin();cit3!=UIDmapr.end();cit3++)
            {
               const MSG_MAP& mapr = cit3->second;
               for (cit4=mapr.begin();cit4!=mapr.end();cit4++)
               {
                  const OrbDataSys* p = cit4->second;
                  p->dumpTerse(s);
                  s << endl;
               }
            }
         }
      }
   } // end OrbSysStore::dumpTerse

//-----------------------------------------------------------------------------
   void OrbSysStore::dumpTerseTimeOrder(std::ostream& s)
         const throw()
   {
      s << "**********************************************************" << endl;
      s << " One-line summary of non-orbit constellation overhead data" << endl;
      s << "       Sat  ID mm/dd/yyyy HH:MM:SS  Data" << endl;
      SAT_NM_UID_MSG_MAP::const_iterator cit1;
      NM_UID_MSG_MAP::const_iterator cit2;
      UID_MSG_MAP::const_iterator cit3;
      MSG_MAP::const_iterator cit4; 

         // Create a list of the NavIDs found in this set of maps.
      set<NavID> navSet;
      set<NavID>::const_iterator cit;
      for (cit1=msgMap.begin();cit1!=msgMap.end();cit1++)
      {
         const NM_UID_MSG_MAP& NMmapr = cit1->second;
         for (cit2=NMmapr.begin();cit2!=NMmapr.end();cit2++)
         {
            const NavID& navType = cit2->first;
            cit = navSet.find(navType);
            if (cit==navSet.end()) navSet.insert(navType);
         }
      }      

      list<SatID> satIDList = getSatIDList();
      list<SatID>::const_iterator csat; 
      typedef map<unsigned short, unsigned long> SUB_MAP;
      
         // For each NavID, build a map<CommonTime, map<SatID.id, UID>>
         // for all the unique messages received.   HEAVEN HELP the user who
         // calls dump( ) for a storeAll map. 
         // Then unspool the multimap to the output stream 
      for (cit=navSet.begin();cit!=navSet.end();cit++)
      {
         bool foundAtLeastOneEntry = false;
         const NavID& navTypeTarget = *cit; 
         multimap<CommonTime, const OrbDataSys*> tempMap;
         for (cit1=msgMap.begin();cit1!=msgMap.end();cit1++)
         {
            const SatID& sidr = cit1->first;
            const NM_UID_MSG_MAP& NMmapr = cit1->second;
            for (cit2=NMmapr.begin();cit2!=NMmapr.end();cit2++)
            {
               const NavID& navType = cit2->first;

                  // If this is not the type of nav we are interested in
                  // skip it. 
               if (navType!=navTypeTarget) continue;

                  // Otherwise, iterate over the submaps
               const UID_MSG_MAP& UIDmapr = cit2->second;
               for (cit3=UIDmapr.begin();cit3!=UIDmapr.end();cit3++)
               {
                  const unsigned long UID = cit3->first;
                  const MSG_MAP& mapr = cit3->second;
                  for (cit4=mapr.begin();cit4!=mapr.end();cit4++)
                  {
                     const CommonTime& ctr = cit4->first;
                     const OrbDataSys* op = cit4->second;
                     multimap<CommonTime, const OrbDataSys*>::value_type inp(ctr,op);
                     tempMap.insert(inp); 
                     foundAtLeastOneEntry = true;
                  }
               }
            }
         } 

            // Only output the table if there is at least one entry
         if (!foundAtLeastOneEntry) continue;

         multimap<CommonTime,const OrbDataSys*>::const_iterator t1;
         for (t1=tempMap.begin();t1!=tempMap.end();t1++)
         {
            const CommonTime& ctr = t1->first;
            const OrbDataSys* op = t1->second;
            op->dumpTerse(s);
            s << endl;
         }
      }
   } // end OrbSysStore::dumpTerseTimeOrdered

//-----------------------------------------------------------------------------
   void OrbSysStore::dumpContents(std::ostream& s,
                                const gpstk::SatID& sidr,
                                const gpstk::NavID& navtype,
                                const unsigned long UID)
         const throw()
   {
      bool allSats = false;
      bool allNM = false;
      bool allUID = false;
      if (sidr.id==0) allSats = true;
      if (navtype.navType==NavID::ntUnknown) allNM = true;
      if (UID==0) allUID = true;

      SAT_NM_UID_MSG_MAP::const_iterator cit1;
      NM_UID_MSG_MAP::const_iterator cit2;
      UID_MSG_MAP::const_iterator cit3;
      MSG_MAP::const_iterator cit4; 
      for (cit1=msgMap.begin();cit1!=msgMap.end();cit1++)
      {
         const SatID& sidCurr = cit1->first;
         if (!allSats && sidCurr!=sidr) continue;
         
         const NM_UID_MSG_MAP& NMmapr = cit1->second;
         for (cit2=NMmapr.begin();cit2!=NMmapr.end();cit2++)
         {
            const NavID& nmCurr = cit2->first;
            if (!allNM && nmCurr!=navtype) continue;
            
            const UID_MSG_MAP& UIDmapr = cit2->second;
            for (cit3=UIDmapr.begin();cit3!=UIDmapr.end();cit3++)
            {
               const unsigned long UIDCurr = cit3->first;
               if (!allUID && UIDCurr!=UID) continue;
               
               const MSG_MAP& mapr = cit3->second;
               for (cit4=mapr.begin();cit4!=mapr.end();cit4++)
               {
                  const OrbDataSys* p = cit4->second;
                  p->dump(s);
               }
            }
         }
      }
   }  // end OrbSysStore::dumpContents


//-----------------------------------------------------------------------------
   unsigned OrbSysStore::size() const
   {
      unsigned counter = 0;

      SAT_NM_UID_MSG_MAP::const_iterator cit1;
      NM_UID_MSG_MAP::const_iterator cit2;
      UID_MSG_MAP::const_iterator cit3;
      for (cit1=msgMap.begin();cit1!=msgMap.end();cit1++)
      {
         const NM_UID_MSG_MAP& NMmapr = cit1->second;
         for (cit2=NMmapr.begin();cit2!=NMmapr.end();cit2++)
         {
            const UID_MSG_MAP& UIDmapr = cit2->second;
            for (cit3=UIDmapr.begin();cit3!=UIDmapr.end();cit3++)
            {
               const MSG_MAP& mapr = cit3->second;
               counter += mapr.size();
            }
         }
      }
      return counter;
   }

//-----------------------------------------------------------------------------
   bool OrbSysStore::isPresent(const SatID& id) const 
   {
      SAT_NM_UID_MSG_MAP::const_iterator cit1;
      cit1 = msgMap.find(id);
      if (cit1==msgMap.end()) return false;
      return true;
   }

//-----------------------------------------------------------------------------
   CommonTime OrbSysStore::getInitialTime() const
         throw(gpstk::InvalidRequest)
   {
      if (size()==0) 
      {
         InvalidRequest ir("No data loaded.");
         GPSTK_THROW(ir); 
      }
      return initialTime; 
   }

//-----------------------------------------------------------------------------
   CommonTime OrbSysStore::getFinalTime() const
         throw(gpstk::InvalidRequest)
   { 
      if (size()==0) 
      {
         InvalidRequest ir("No data loaded.");
         GPSTK_THROW(ir); 
      }
      return finalTime; 
   }

  
//-----------------------------------------------------------------------------
/*
*  The following diagram illustrates the intent of the find process.
*
* Test  X1    X2      X3    X4        X5     X6    X7
* Case   v    v       v     v         v      v     v
*    ---------------------------------------------------------   Time
* Data        ^             ^                ^
* Loaded      T1            T2               T3           
*           (begin)                                         (end)
*
*  Desired Results
*
*   Test  
*   Case    Result
*   ----    ------
*      1    Invalid Requrest (too early)
*      2    Invalid Requrest (too early)
*      3    T1
*      4    T1
*      5    T2
*      6    T2
*      7    T3
*
*  Since there is no end time and no fit interval, there is no way to 
*  say a message is "too old".  This needs to be considered in the 
*  calling program.
*/
   const OrbDataSys* OrbSysStore::
   find(const SatID& sat, 
        const NavID& navtype,   // Transition to NavID when available
        const unsigned long UID, 
        const CommonTime& t) const
      throw(InvalidRequest)
   {
      SAT_NM_UID_MSG_MAP::const_iterator cit1;
      NM_UID_MSG_MAP::const_iterator cit2;
      UID_MSG_MAP::const_iterator cit3;
      MSG_MAP::const_iterator cit4;

         // First step is to establish if there are any messages
         // in the store matching the request satellite, nav message 
         // type and unique ID. If any of these fail, InvalidRequest
         // is thrown.
      stringstream failString;
      cit1 = msgMap.find(sat);
      if (cit1==msgMap.end())
      {
         failString << "Satellite " << sat << " not found in message store.";
         InvalidRequest ir(failString.str());
         GPSTK_THROW(ir);
      }

      const NM_UID_MSG_MAP& NMmapr = cit1->second;
      cit2 = NMmapr.find(navtype);
      if (cit2==NMmapr.end())
      {
         failString << "Nav message type " << navtype << " not found in message store.";
         InvalidRequest ir(failString.str());
         GPSTK_THROW(ir);
      }

      const UID_MSG_MAP& UIDmapr = cit2->second;
      cit3 = UIDmapr.find(UID);
      if (cit3==UIDmapr.end())
      {
         failString << "Unique message ID " << UID << " not found in message store.";
         InvalidRequest ir(failString.str());
         GPSTK_THROW(ir);
      }

         // Found a map of candidate messages.
      const MSG_MAP& mapr = cit3->second;
      
         // The map is ordered by transmit time.  In the typical 
         // case of a "sparse" map that stores only the first copy
         // of each unique message, that time SHOULD be the earliest
         // transmit time. 
         //
         // Recall that the transmit time marks the BEGINNING of the
         // transmission of the message.  Therefore, a "direct match"
         // of times should actually use the PRIOR message (if one is
         // available). 
         //
         // First, check if time t is prior to any useful messages
         // in this map. 
      cit4 = mapr.begin();
      const CommonTime& ctr = cit4->first;

      string tform = "%02m/%02d/%4Y %02H:%02M:%02S";
      if (debugLevel)
      {
         cout << "   t: " << printTime(t,tform) << ", " << sat << endl;
         cout << " ctr: " << printTime(ctr,tform) << endl;
      }
      
      if (t<=ctr)
      {
         stringstream ss;
         ss << "Requested time is earlier than any message of requested type."; 
         InvalidRequest ir(ss.str());
         GPSTK_THROW(ir);
      }      

         // Now we know that time t is beyod the beginning of the map.
         // The upper_bound() method will return the first message with a key
         // BEYOND time t.
      MSG_MAP::const_iterator exact, upper, prior;
      upper = mapr.upper_bound(t); 

         // If we are at the end of the map, the requested
         // time is either between entry n-1 and n or after n 
         // where n is the final entry. 
      if (upper==mapr.end())
      {
         if (debugLevel) cout << "Ran off end of map." << endl;
         MSG_MAP::const_reverse_iterator rit = mapr.rbegin();

         const CommonTime& lastXmit = rit->first;
         if (t>lastXmit) return rit->second;

         rit++; 
         if (rit!=mapr.rend()) return rit->second;

         stringstream ss;
         ss << "Could not find object with appropriate transmit time.";
         InvalidRequest ir(ss.str());
         GPSTK_THROW(ir); 
      }

         // If not at the end, retreat one item. 
      if (debugLevel)
      {
	      cout << "Time associated with pointer upper: " << printTime(upper->first,tform) << endl;
         cout << "Retreating one entry" << endl; 
      }
      prior = upper;
      prior--;         // guaranteed to succeed due to earlier test
      const CommonTime& priorCT = prior->first;

         // As long as the time of the prior object is less than time t, 
         // the object associated with prior is the one we want.
      if (debugLevel)
      {
         cout << "priorCT : " << printTime(priorCT,tform) << endl;
      }
      if (priorCT<t) return prior->second; 

         // If priorCT==t, then we need to back up one more
      if (debugLevel) cout << "Attempting to retreating a second time" << endl;
      if (prior==mapr.begin())
      {
	 if (debugLevel) cout << "...failed to retreat.  Already at beginning" << endl;
         stringstream ss;
         ss << "Requested time is earlier than any message of requested type."; 
         InvalidRequest ir(ss.str());
         GPSTK_THROW(ir);
      }
      prior--;
      if (debugLevel) cout << "Returning object with xmit time: "
	                   << printTime(prior->first,tform) << endl;
      return prior->second;
   }
         
//-----------------------------------------------------------------------------
   std::list<const OrbDataSys*> OrbSysStore::
   findSystemData(const SatID& sat,
                  const NavID& navtype,   // Transition to NavID when available
                  const CommonTime& t) const
      throw(InvalidRequest)
   {
      SAT_NM_UID_MSG_MAP::const_iterator cit1;
      NM_UID_MSG_MAP::const_iterator cit2;
      UID_MSG_MAP::const_iterator cit3;
      MSG_MAP::const_iterator cit4;

         // First step is to establish if there are any messages
         // in the store matching the request satellite, nav message 
         // type and unique ID. If any of these fail, InvalidRequest
         // is thrown.
      stringstream failString;
      cit1 = msgMap.find(sat);
      if (cit1==msgMap.end())
      {
         failString << "Satellite " << sat << " not found in message store.";
         InvalidRequest ir(failString.str());
         GPSTK_THROW(ir);
      }

      const NM_UID_MSG_MAP& NMmapr = cit1->second;
      cit2 = NMmapr.find(navtype);
      if (cit2==NMmapr.end())
      {
         failString << "Nav message type " << navtype << " not found in message store.";
         InvalidRequest ir(failString.str());
         GPSTK_THROW(ir);
      }

         // Iterate over each UID in the store for this message type
      list<unsigned long> UIDList;
      const UID_MSG_MAP& UIDmapr = cit2->second; 
      for (cit3=UIDmapr.begin(); cit3!=UIDmapr.end(); cit3++)
      {
         UIDList.push_back(cit3->first);
      }

      list<const OrbDataSys*> retList;
      list<unsigned long>::const_iterator cit;
      for (cit=UIDList.begin();cit!=UIDList.end();cit++)
      {
         unsigned long UID = *cit;
         try 
         {
            retList.push_back(find(sat,navtype,UID,t));
         }
         catch (InvalidRequest ir)
         {
            GPSTK_RETHROW(ir);
         }
      }
      return retList;    
   }

//-----------------------------------------------------------------------------
   std::list<const OrbDataSys*> OrbSysStore::findList(const SatID& sat, 
                                                      const NavID& navtype,
                                                      const unsigned long UID) const
         throw(InvalidRequest)
   {
      SAT_NM_UID_MSG_MAP::const_iterator cit1;
      NM_UID_MSG_MAP::const_iterator cit2;
      UID_MSG_MAP::const_iterator cit3;
      MSG_MAP::const_iterator cit4;

         // First step is to establish if there are any messages
         // in the store matching the request satellite, nav message 
         // type and unique ID. If any of these fail, InvalidRequest
         // is thrown.
      stringstream failString;
      cit1 = msgMap.find(sat);
      if (cit1==msgMap.end())
      {
         failString << "Satellite " << sat << " not found in message store.";
         InvalidRequest ir(failString.str());
         GPSTK_THROW(ir);
      }

      const NM_UID_MSG_MAP& NMmapr = cit1->second;
      cit2 = NMmapr.find(navtype);
      if (cit2==NMmapr.end())
      {
         failString << "Nav message type " << navtype << " not found in message store.";
         InvalidRequest ir(failString.str());
         GPSTK_THROW(ir);
      }

      const UID_MSG_MAP& UIDmapr = cit2->second;
      cit3 = UIDmapr.find(UID);
      if (cit3==UIDmapr.end())
      {
         failString << "Uniuqe message type " << UID << " not found in message store.";
         InvalidRequest ir(failString.str());
         GPSTK_THROW(ir);
      }

         // Iterate over the time-ordered message map and copy all the messages 
         // into the list to be returned.
      list<const OrbDataSys*> retList;
      const MSG_MAP& mapr = cit3->second;
      for (cit4=mapr.begin();cit4!=mapr.end();cit4++)
      {
         const OrbDataSys* cp = cit4->second;
         retList.push_back(cp); 
      }
      return retList;    
   }

//-----------------------------------------------------------------------------
// Remove all data from this collection.
   void OrbSysStore::clear()
         throw()
   {
      SAT_NM_UID_MSG_MAP::iterator it1;
      NM_UID_MSG_MAP::iterator it2;
      UID_MSG_MAP::iterator it3;
      MSG_MAP::iterator it4;
      for (it1=msgMap.begin();it1!=msgMap.end();it1++)
      {
         NM_UID_MSG_MAP& NMmapr = it1->second;
         for (it2=NMmapr.begin();it2!=NMmapr.end();it2++)
         {
            UID_MSG_MAP& UIDmapr = it2->second;
            for (it3=UIDmapr.begin();it3!=UIDmapr.end();it3++)
            {
               MSG_MAP& mapr = it3->second;
               for (it4=mapr.begin();it4!=mapr.end();it4++)
               {
                  OrbDataSys* odsp = it4->second;
                  delete odsp; 
               }
               mapr.clear();
            }
            UIDmapr.clear();
         }
         NMmapr.clear();
      }
      msgMap.clear();
      initialTime = gpstk::CommonTime::END_OF_TIME;
      finalTime = gpstk::CommonTime::BEGINNING_OF_TIME;
      initialTime.setTimeSystem(timeSysForStore);
      finalTime.setTimeSystem(timeSysForStore); 
   }

//-----------------------------------------------------------------------------
   list<gpstk::SatID> OrbSysStore::getSatIDList() const
   {
      list<gpstk::SatID> retList;
      SAT_NM_UID_MSG_MAP::const_iterator cit1;
      for (cit1=msgMap.begin();cit1!=msgMap.end();cit1++)
      {
         SatID sid = cit1->first;
         retList.push_back(sid);
      } 
      return retList;
   }


//-----------------------------------------------------------------------------
   bool OrbSysStore::isSatSysPresent(const SatID::SatelliteSystem ss) const
   {
      list<SatID::SatelliteSystem>::const_iterator cit;
      for (cit=sysList.begin();cit!=sysList.end();cit++)
      {
         SatID::SatelliteSystem ssTest = *cit;
         if (ssTest==ss) return true;
      }
      return false;
   }
   
//-----------------------------------------------------------------------------
   void OrbSysStore::addSatSys(const SatID::SatelliteSystem ss)
   {
      sysList.push_back(ss);
   }

//-----------------------------------------------------------------------------
   void OrbSysStore::validSatSystem(const SatID& sat) const 
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
   
   
} // namespace
