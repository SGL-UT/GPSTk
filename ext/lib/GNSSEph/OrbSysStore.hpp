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
 * @file OrbSysStore.hpp
 * Base class for storing collection of GNSS navigation messages that 
 * are NOT related to orbit determination. 
 */
 
#ifndef GPSTK_ORBSYSSTORE_HPP
#define GPSTK_ORBSYSSTORE_HPP

#include <iostream>
#include <list>
#include <map>

#include "CommonTime.hpp"
#include "Exception.hpp"
#include "OrbDataSys.hpp"
#include "PackedNavBits.hpp"
#include "NavID.hpp"
#include "SatID.hpp"
#include "TimeSystem.hpp"

namespace gpstk
{
   class OrbSysStore 
   {
   public:
      
      OrbSysStore(const bool storeAllArg=false)
         throw()
        :initialTime(CommonTime::END_OF_TIME), 
         finalTime(CommonTime::BEGINNING_OF_TIME),
         timeSysForStore(TimeSystem::Any),
         debugLevel(0)
      {
         initialTime.setTimeSystem(timeSysForStore);
         finalTime.setTimeSystem(timeSysForStore);
         storeAll = storeAllArg;
      }

      virtual ~OrbSysStore()
      {}

      virtual bool addMessage(const OrbDataSys* eph)
         throw(InvalidRequest,Exception);

     // virtual bool addMessage(const PackedNavBits& pnb)
     //    throw(InvalidRequest,Exception);

      virtual void deleteMessage(const SatID& sat, 
                             const NavID& navtype,
                             const unsigned long UID, 
                             const CommonTime& t);

      /// A debugging function that outputs in human readable form,
      /// all data stored in this object.
      /// @param[in] s the stream to receive the output; defaults to cout
      /// @param[in] detail the level of detail to provide
      virtual void dump(std::ostream& s = std::cout, short detail = 0)
         const throw();

      virtual void dumpTerse(std::ostream& s = std::cout)
         const throw();

      virtual void dumpTerseTimeOrder(std::ostream& s = std::cout)
         const throw();

      virtual void dumpContents(std::ostream& s = std::cout,
                                const gpstk::SatID& sidr =
                                      gpstk::SatID(0,gpstk::SatID::systemUnknown),
                                const gpstk::NavID& navtype =
                                      gpstk::NavID(gpstk::NavID::ntUnknown),
                                const unsigned long UID = 0)
         const throw();


      /// Clear the dataset, meaning remove all data
      virtual void clear(void) throw();

      /// Determine the earliest time for which this object can successfully 
      /// determine the Xvt for any object.
      /// @return The initial time
      /// @throw InvalidRequest This is thrown if the object has no data.
      virtual CommonTime getInitialTime() const
         throw(gpstk::InvalidRequest);

      /// Determine the latest time for which this object can successfully 
      /// determine the Xvt for any object.
      /// @return The final time
      /// @throw InvalidRequest This is thrown if the object has no data.
      virtual CommonTime getFinalTime() const
         throw(gpstk::InvalidRequest);

      /// Return the number of messages stored in this store. 
      virtual unsigned size() const; 

      /// Return true if the given SatID is present in the store
      virtual bool isPresent(const SatID& id) const;

      /// Classes to set/access the store TimeSystem information.
      TimeSystem getTimeSystem() const { return timeSysForStore; }
      void setTimeSystem(const TimeSystem ts) { timeSysForStore = ts; }

      /// Classes to set/test the satellite system list.
      bool isSatSysPresent(const SatID::SatelliteSystem ss) const;
      void addSatSys(const SatID::SatelliteSystem ss); 
      void validSatSystem(const SatID& sat) const 
         throw( InvalidRequest );

      /// Given a satellite, a system-specific message unique ID (UID),
      /// and a time, return a pointer to the message that was being
      /// broadcast by the specified satellite at the specified time.
      /// Throws InvalidRequest if
      ///   - the satellite isn't in the store
      ///   - if the time is prior to the earliest time in the store
      ///   - if there are no message with the specified UID in the store
      const OrbDataSys* find(const SatID& sat, 
                             const NavID& navtype,
                             const unsigned long UID, 
                             const CommonTime& t) const
         throw(InvalidRequest);
         
      /// Given a satellite and a time, return list of pointer to the 
      /// messages that were being
      /// broadcast by the specified satellite at the specified time.
      /// Throws InvalidRequest if
      ///   - the satellite isn't in the store
      ///   - if the time is prior to the earliest time in the store
      std::list<const OrbDataSys*> findSystemData(const SatID& sat,
                                                  const NavID& navtype, 
                                                  const CommonTime& t) const
         throw(InvalidRequest);

      /// Given a satellite, a message type and a system-specific 
      /// message unique ID (UID),
      /// return a list of the messages in the store and fit the
      /// description.
      /// Throws InvalidRequest if
      ///   - the satellite isn't in the store
      //    - the NavID isn't in the store
      ///   - if there are no message with the specified UID in the store
      std::list<const OrbDataSys*> findList(const SatID& sat, 
                                            const NavID& navtype,
                                            const unsigned long UID) const
         throw(InvalidRequest);

      /// Return a list of SatID object representing the satellites that
      /// are contained in the store.
      /// @return list of SatID objects
      std::list<gpstk::SatID> getSatIDList() const;

      unsigned int setDebugLevel(const unsigned int newLevel)
         { debugLevel = newLevel; return debugLevel; }

      unsigned int debugLevel;

      /// The map is an hierarchical structure that is ranked by
      /// satellite, then navigation mesage type, then message UID 
      /// and finally by transmit time.
      ///
      ///                      key,    value,    MAP
      ///  -----------------------   --------    ----
      ///            transmit time,  message,    MSG_MAP
      ///        unique message ID,              UID_MSG_MAP
      ///  navigation message type,              NAV_UID_MSG_MAP
      ///                satellite,              SAT_NM_UID_MSG_MAP
      ///
      typedef std::map<gpstk::CommonTime, gpstk::OrbDataSys*> MSG_MAP;
      typedef std::map<uint16_t, MSG_MAP> UID_MSG_MAP;
      typedef std::map<gpstk::NavID, UID_MSG_MAP> NM_UID_MSG_MAP;
      typedef std::map<gpstk::SatID,NM_UID_MSG_MAP> SAT_NM_UID_MSG_MAP; 

      protected:

         // If storeAll is set, every message provided via addMessage()
         // will be stored.   If not, only the unique messages (i.e. data
         // payload unique) will be stored. 
      bool storeAll; 
     
         // The map where all messages are stored.
      SAT_NM_UID_MSG_MAP msgMap;

         // NOTE: The concept of "final time" in this store is NOT CONSISTENT
         // with the concept of final time in the OrbElemStore.   In this
         // case, there is no fit interval. Therefore there is no way to 
         // define a "final time of effectivity".  Instead, the final time
         // records the latest transmit time stored in the store. 
      CommonTime initialTime; //< xmit time of the first message in the store
      CommonTime finalTime;   //< xmit time of the last message in the store

         // List of the satellite systems stored in this store.  Typically
         // only one and set by descendents. 
      std::list<SatID::SatelliteSystem> sysList;
      
         // TimeSystem used in this store.  Set by default to "Any", but
         // typically overridden by descendents. 
      TimeSystem timeSysForStore; 

      // Here is a method to simplify the .cpp
      void updateInitialTime(const OrbDataSys* ods)
      {
        if (ods->beginValid<initialTime)       
          initialTime = ods->beginValid;
         
        if (ods->beginValid>finalTime)               
          finalTime = ods->beginValid;
      }

   }; // end class

   //@}

} // namespace

#endif
