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
 * @file OrbElemStore.hpp
 * Base class for storing and/or computing position, velocity, 
 * and clock data.  This class is typically extended in order
 * to implement SV-system specific behavior.  Theoretically, 
 * it could be used to store OrbElemBase objects across multiple
 * systems, but that would entail moving all the time tags to a 
 * common system. 
 */
 
#ifndef GPSTK_ORBELEMSTORE_HPP
#define GPSTK_ORBELEMSTORE_HPP

#include <iostream>
#include <list>
#include <map>

#include "CommonTime.hpp"
#include "Exception.hpp"
#include "OrbElemBase.hpp"
#include "SatID.hpp"
#include "TimeSystem.hpp"
#include "XvtStore.hpp"

namespace gpstk
{
   /// @ingroup ephemstore 
   //@{

   /// Base class for storing and accessing an objects position, 
   /// velocity, and clock data. Also defines a simple interface to remove
   /// data that has been added.
   class OrbElemStore : public XvtStore<SatID>
   {
   public:
      
      OrbElemStore()
         throw()
        :initialTime(CommonTime::END_OF_TIME), 
         finalTime(CommonTime::BEGINNING_OF_TIME),
         timeSysForStore(TimeSystem::Any)
      {
         initialTime.setTimeSystem(timeSysForStore);
         finalTime.setTimeSystem(timeSysForStore);
      }

      virtual ~OrbElemStore()
      {}

      /// Returns the position, velocity, and clock offset of the indicated
      /// object in ECEF coordinates (meters) at the indicated time.
      /// @param[in] id the object's identifier
      /// @param[in] t the time to look up
      /// @return the Xvt of the object at the indicated time
      /// @throw InvalidRequest If the request can not be completed for any
      ///    reason, this is thrown. The text may have additional
      ///    information as to why the request failed.
      virtual Xvt getXvt(const SatID& id, const CommonTime& t)
         const throw( InvalidRequest );

      /// A debugging function that outputs in human readable form,
      /// all data stored in this object.
      /// @param[in] s the stream to receive the output; defaults to cout
      /// @param[in] detail the level of detail to provide
      virtual void dump(std::ostream& s = std::cout, short detail = 0)
         const throw();

      ///
      ///   
      virtual bool isHealthy(const SatID& sat, const CommonTime& t) 
         const throw(InvalidRequest); 

      virtual bool addOrbElem(const OrbElemBase* eph)
         throw(InvalidParameter,Exception);

      /// Edit the dataset, removing data outside the indicated time interval
      /// @param[in] tmin defines the beginning of the time interval
      /// @param[in] tmax defines the end of the time interval
      virtual void edit(const CommonTime& tmin, 
                        const CommonTime& tmax = CommonTime::END_OF_TIME)
         throw(); 

      /// Clear the dataset, meaning remove all data
      virtual void clear(void) throw();

      /// Determine the earliest time for which this object can successfully 
      /// determine the Xvt for any object.
      /// @return The initial time
      /// @throw InvalidRequest This is thrown if the object has no data.
      virtual CommonTime getInitialTime() const
         throw()
         { return initialTime; }


      /// Determine the latest time for which this object can successfully 
      /// determine the Xvt for any object.
      /// @return The final time
      /// @throw InvalidRequest This is thrown if the object has no data.
      virtual CommonTime getFinalTime() const
         throw()
         { return finalTime; }

      /// Return the number of orbit/clock elements stored in this store. 
      virtual unsigned size() const
         throw(); 

      virtual bool velocityIsPresent()
         const throw()
         { return true; }

      /// Return true if velocity data is present in the store
      virtual bool hasVelocity() const throw()
         { return true; }

      /// Return true if the given SatID is present in the store
      virtual bool isPresent(const SatID& id) const throw();

      /// Classes to set/access the store TimeSystem information.
      TimeSystem getTimeSystem() const { return timeSysForStore; }
      void setTimeSystem(const TimeSystem ts) { timeSysForStore = ts; }

      /// Classes to set/test the satellite system list.
      bool isSatSysPresent(const SatID::SatelliteSystem ss) const;
      void addSatSys(const SatID::SatelliteSystem ss); 
      void validSatSystem(const SatID& sat) const 
         throw( InvalidRequest );
      /*
       *  Explanation of find( ) function for OrbElemStore
       *  
       *  The findOrbElem( ) funtion
       *  does the best possible job of emulating the choice
       *  that would be made by a real-time user
       *
       *  It is strongly suggested that the user load ALL 
       *  available set of orbital elements into the store, 
       *  regardless of health status.  
       */
      /// @param sat SatID of satellite of interest
      /// @param t time with which to search for OrbElemBase
      /// @return a reference to the desired OrbElemBase
      /// @throw InvalidRequest object thrown when no OrbElemBase is found
      const OrbElemBase* findOrbElem( const SatID& sat, const CommonTime& t )
         const throw( InvalidRequest );

      /// Find an OrbElemBase for the indicated satellite at time t. The OrbElemBase
      /// chosen is the one with HOW time closest to the time t, (i.e. with
      /// smallest fabs(t-HOW), but still within the fit interval.
      /// @param sat the SV of interest
      /// @param t the time of interest
      /// @return a reference to desired OrbElemBase
      /// @throw InvalidRequest object thrown when no OrbElemBase is found
      const OrbElemBase* findNearOrbElem( const SatID& sat, const CommonTime& t )
         const throw( InvalidRequest );

      /// Find an OrbElemBase for the indicated satellite that has a Toe
      /// corresponding to time t.  If no such OrbElemBase exists in the store,
      /// throw an InvalidRequest exception
      /// @param sat the SV of interest
      /// @param t the time of interest
      /// @return a reference to desired OrbElemBase
      /// @throw InvalidRequest object thrown when no OrbElemBase is found
      const OrbElemBase* findToe(const SatID& sat, const CommonTime& t)
         const throw(InvalidRequest);

      /// Return a list of SatID object representing the satellites that
      /// are contained in the store.
      /// @return list of SatID objects
      std::list<gpstk::SatID> getSatIDList() const;

      /// Add all ephemerides to an existing list<OrbElemBase>.
      /// @return the number of ephemerides added.
      int addToList( std::list<OrbElemBase*>& v ) const
         throw();

      /// This is intended to store sets of unique orbital elements for a single SV.
      /// The key is the beginning of the period of validity for each set of elements. 
      typedef std::map<CommonTime, OrbElemBase*> OrbElemMap;

      /// Returns a map of the ephemerides available for the specified
      /// satellite.  Note that the return is specifically chosen as a
      /// const reference.  The intent is to provide "read only" access
      /// for analysis.  If the map needs to be modified, see other methods.
      const OrbElemMap& getOrbElemMap( const SatID& sat ) const
         throw( InvalidRequest );

      protected:
     
      /// This is intended to hold all unique OrbElemBase objects for each SV
      /// The key is the prn of the SV.
      typedef std::map<SatID, OrbElemMap> UBEMap;

      /// The map where all EngEphemerides are stored.
      UBEMap ube;

      CommonTime initialTime; //< Time of the first OrbElemBase in the store
      CommonTime finalTime;   //< Time of the last OrbElemBase in the store

         // List of the satellite systems stored in this store.  Typically
         // only one and set by descendents. 
      std::list<SatID::SatelliteSystem> sysList;
      
         // TimeSystem used in this store.  Set by default to "Any", but
         // typically overridden by descendents. 
      TimeSystem timeSysForStore; 

      // Here is a method to simplify the .cpp
      void updateInitialFinal(const OrbElemBase* eph)
      {
        if (eph->beginValid<initialTime)       
          initialTime = eph->beginValid;
         
        if (eph->endValid>finalTime)               
          finalTime = eph->endValid;
      }

   }; // end class

   //@}

} // namespace

#endif
