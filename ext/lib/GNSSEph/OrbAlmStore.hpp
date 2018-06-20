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
/*
*   OrbAlmStore.hpp
*
*   Typically handling of almanac data is somewhat different than handling 
*   real-time broadcast orbit/clock information.   Depending on the circumstances,
*   we may care about any of the following:
*     - What is the collection of almanac data available from a given SV at 
*       a given time?
*     - What is the most recent alamanc data available for a given SV regardless
*       of the SV from which it was received.
*     - How many unique almanacs are available at a given time. 
*
*   OrbAlmStore is to some extent parallel to OrbElemStore, but attempts to
*   address each of these use cases. 
*
*   Throughout the interface there are references to two types of satellite
*   identifiers.
*     satID     - The SatID of the satellite that broadcast an almanac (regardless
*                  of the satellite to which the almanac applies). 
*     subjectSV - The SatID of the satellite ot which the almanac applies. 
*
*/
#ifndef SGLTK_ORBALMSTORE_HPP
#define SGLTK_ORBALMSTORE_HPP

#include <iostream>
#include <list>
#include <map>

#include "OrbAlm.hpp"
#include "Exception.hpp"
#include "OrbAlmFactory.hpp"
#include "SatID.hpp"
#include "CommonTime.hpp"
#include "XvtStore.hpp"
#include "TimeSystem.hpp"

namespace gpstk
{
   /** @addtogroup almemstore */
   //@{

   /// Class for storing and accessing GPS SV's position, 
   /// velocity, and clock data. Also defines a simple interface to remove
   /// data that has been added.
   class OrbAlmStore : public gpstk::XvtStore<SatID>
   {
   public:
      
      OrbAlmStore()
         throw()
         : initialTime(CommonTime::END_OF_TIME), 
           finalTime(CommonTime::BEGINNING_OF_TIME)
      {
         initialTime.setTimeSystem(TimeSystem::Any);
         finalTime.setTimeSystem(TimeSystem::Any);
      }

      virtual ~OrbAlmStore()
      { clear();}

      /// Returns the position, velocity, and clock offset of the indicated
      /// satellite in ECEF coordinates (meters) at the indicated time.
      /// @param[in] sat the SV's SatID
      /// @param[in] t the time to look up
      /// @return the Xvt of the SV at time
      /// @throw InvalidRequest If the request can not be completed for any
      ///    reason, this is thrown. The text may have additional
      ///    information as to why the request failed.  The most common reason
      ///    is that no orbital elements are stored for the SV.
      virtual Xvt getXvt( const SatID& subjID, const CommonTime& t ) const
         throw( InvalidRequest );


      /// As getXvt( ) with the additional check that the elements used
      /// must have a fit interval that covers the requested time.  If not,
      /// InvalidRequest is thrown. 
      virtual Xvt getXvt_WithinValidity( const SatID& subjID, const CommonTime& t ) const
         throw( InvalidRequest );


      /// A debugging function that outputs in human readable form,
      /// all data stored in this object.
      /// @param[in] s the stream to receive the output; defaults to cout
      /// @param[in] detail the level of detail to provide
      virtual void dump( std::ostream& s = std::cout, short detail = 0 ) const
         throw();

      /// Edit the dataset, removing data outside the indicated time interval
      /// @param tmin defines the beginning of the time interval, included
      /// @param tmax defines the end of the time interval. not included
      /// [tmin, tmax)
      virtual void edit( const CommonTime& tmin, 
                         const CommonTime& tmax = CommonTime::END_OF_TIME )
         throw();

      /// Return time system 
      virtual TimeSystem getTimeSystem(void) const throw()
         { return TimeSystem::Any; }

      /// Determine the earliest time for which this object can successfully 
      /// determine the Xvt for any satellite.
      /// @return The initial time
      /// @throw InvalidRequest This is thrown if the object has no data.
      virtual CommonTime getInitialTime() const
         throw()
         { return initialTime; }


      /// Determine the latest time for which this object can successfully
      /// determine the Xvt for any satellite.
      /// @return The final time
      /// @throw InvalidRequest This is thrown if the object has no data.
      virtual CommonTime getFinalTime() const
         throw()
         { return finalTime; }

      virtual bool velocityIsPresent()
         const throw()
      { return true; }

      /// Return true if velocity data is present in the store
      virtual bool hasVelocity() const throw()
      { return true; }

      /// Return true if the given IndexType is present in the store
      virtual bool isPresent(const SatID& sat) const throw()
      {
         if(subjectAlmMap.find(sat) != subjectAlmMap.end()) return true;
         return false;
      }

      //---------------------------------------------------------------
      // Below are interfaces that are unique to this class (i.e. not
      // in the parent class)
      //---------------------------------------------------------------

      virtual bool validSatSystem(const SatID& subjID) const;

      /// Returns the health of an SV for a particular time.
      /// @param sat the satellite's SatID
      /// @param t the time to look up
      /// @return the SV health bits
      /// @throw InvalidRequest no matching OrbElem found in the store
      virtual bool isHealthy( const SatID& subjID, const CommonTime& t ) const
         throw( InvalidRequest );

      /// Convenience method.  Since navigation message data frequently
      /// is stored in PackedNavBits objects, provide a means to go
      /// directly from PNB into the map.  
      virtual unsigned short addMessage(const PackedNavBits& pnb)
         throw(InvalidParameter,Exception);

      /// Add an OrbAlm object to this collection. 
      /// Note: There are actually TWO collections.  A collection per-SV and a 
      /// collection aggregated across all SVs.   The health of the
      /// transmitted SV is passed to the almanac store
      /// loader in order that almanac data from unhealthy
      /// SVs is not accepted into the aggregation. 
      /// @param alm the OrbAlm (OrbAlm) to add
      /// @param isXmitHealthy health status of the transmitting SV,
      /// @return true if OrbAlm was added, false otherwise
      virtual unsigned short addOrbAlm( const OrbAlm* alm,
                              const bool isXmitHealthy=true )
         throw(InvalidParameter,Exception);

      /// Remove all data from this collection.
      virtual void clear()
         throw();

      /// Get the number of OrbAlm objects in this collection.
      /// @return the number of OrbAlm records in the map
      /// The switch argument is provided in order to differentiate
      /// between the size of the "xmit almanac" store and the 
      /// "subject almanac" store.
      ///   Switch   Meaning
      ///        0   Determine size of BOTH stores and return the 
      ///            total.  Useful for determining memory requirements,
      ///            but will double count (or more).
      ///        1   Return the size of the "subject almanac" store.
      ///        2   Return the size of the "transmitted almanac" store. 
      virtual unsigned size(unsigned short choice = 0) const
         throw();
      virtual unsigned sizeSubjAlm(const SatID& subjID) const
         throw();
      virtual unsigned sizeXmitAlm(const SatID& xmitID) const
         throw(); 

      virtual void dumpSubjAlm( std::ostream& s = std::cout, 
                                short detail = 0, 
                                const SatID& subjID=SatID() ) const
         throw(InvalidRequest);
      virtual void dumpXmitAlm( std::ostream& s = std::cout, 
                                short detail = 0,
                                const SatID& subjID=SatID() ) const
         throw(InvalidRequest);




      /// This is intended to store sets of unique almanac data. 
      /// The key is the epoch time.
      /// This map is used in two very different ways within this class.
      ///   1. With respect to the subject almanac map, there is OrbElemMap
      ///      for each subject satellite.
      ///   2. With respect to the xmit alamanc map, there is ONE map for
      ///      each transmitting SV that contains all the almanac data 
      ///      received from that SV regardless of the subject satellite. 
      ///
      /// Note that this is a multimap.  In the case of GLONASS and BeiDou,
      /// it has been observed that multiple almanacs are broadcast with the
      /// same epoch time but slightly different contents.   Therefore, 
      /// they will all be stored, but only one copy (the earliest) of
      /// each unique data set will be stored. 
      ///
      /// NOTE: key is toa (not beginValid).  This was originally due to fact
      /// we thought toa might be unique.  In the event, the code to add an
      /// almanac check isSameData( ) to determine uniqueness.   Therefore,
      /// it MIGHT be possible to reduce this to a set or list, but we will
      /// leave that for another time. 
      typedef std::multimap<CommonTime, OrbAlm*> OrbAlmMap;

      /*
       *  Explanation of find( ) function for OrbAlmStore
       *  
       *  The find( ) funtion
       *  does the best possible job of emulating the choice
       *  that would be made by a real-time user
       *
       *  It is strongly suggested that the user load ALL 
       *  available set of orbital elements into the store, 
       *  regardless of health status.  
       */
         // Determine best almanac for SV subjID collected from SV xmitID. 
         // If xmitID == SatID (an invalid value), search the subject
         // almanac map instead of a transmit-SV-specific map.  That is to 
         // say, use the simpler versino of find()..
      const OrbAlm* find( const SatID& subjID, 
                          const CommonTime& t,
                          const bool useEffectivity = true,
                          const SatID& xmitID = SatID() )
         const throw( InvalidRequest );

      /*
       *  Given an OrbAlm pointer, find the OrbAlm in the subject
       *  map.  Then, using the xmit map, derive the probable last
       *  time that particular almanac was broadcast by any SV.
       *  Notes:
       *   - This assumes the OrbAlmStore was loaded in such a 
       *     manner that the earliest transmission of each
       *     unique almanac for each SV is present
       *   - If oap doesn't correspond to any item in the
       *     store, throw an exception.
       *   - If oap is the final item in the store for any
       *     SV, return CommonTime::END_OF_TIME.
       */
      CommonTime deriveLastXmit(const OrbAlm* oap) const
         throw(InvalidRequest); 

      /*
       *  Given an OrbAlm pointer, return a list of the SVs that
       *  transmitted an almanac data set corresponding to the
       *  data in this set.
       */
      std::list<SatID> xmitBySVs(const OrbAlm* oap) const
         throw(InvalidRequest); 

      /*
       * Return a list of all the subject SVs found in the set
       */
      std::list<SatID> listOfSubjectSV() const; 

      /// Returns a map of the almemerides available for the specified
      /// satellite.  Note that the return is specifically chosen as a
      /// const reference.  The intent is to provide "read only" access
      /// for analysis.  If the map needs to be modified, see other methods.
      ///
      /// This version works from the "subject SV map"
      const OrbAlmMap& getOrbAlmMap( const SatID& subjID ) const
         throw( InvalidRequest );
      ///
      /// This version works from the "xmit SV map of subject SV maps"
      const OrbAlmMap&getOrbAlmMap(const SatID& xmitID, 
                                   const SatID& subjID) const
         throw( InvalidRequest );

      void setDebugLevel(const int newLevel) 
      {
         debugLevel = newLevel; 
         orbAlmFactory.debugLevel = newLevel;
      }

      int debugLevel;

         // Variables for returning whether an almanac was added to one or both collections.
      static const unsigned short ADD_NEITHER; 
      static const unsigned short ADD_BOTH;
      static const unsigned short ADD_XMIT;
      static const unsigned short ADD_SUBJ;

      //---------------------------------------------------------------------------
      protected:
      bool addOrbAlmToOrbAlmMap( const OrbAlm* alm, 
                                         OrbAlmMap& oem)
         throw(gpstk::InvalidParameter, gpstk::Exception);

      /// See the public find( ) methods for external access.  This version
      /// is used internally once the correct OrbAlmMap has been identifited.
      ///    @param em an OrbAlmMap containing OrbAlm* for the satellite of interest
      ///    @param t time with which to search for OrbAlm
      ///    @return a pointer to the desired OrbAlm
      ///    @throw InvalidRequest object thrown when no OrbAlm is found
      const OrbAlm* find(const OrbAlmMap& em, 
                     const CommonTime& t,
                     const bool useEffectivity) const
      throw( InvalidRequest );

      /// Returns a string that will be used as a header for
      /// tables that print out OrbAlm::dumpTerse() 
      /// one-line summaries. 
      virtual std::string getTerseHeader() const;
      
      /// This is intended to hold all unique almanacs for each SV
      /// sorted by the subject satellites ID. 
      typedef std::map<SatID, OrbAlmMap> SubjectAlmMap;

      // The map where unique almanacs across all transmitting SVs
      // are stored. 
      SubjectAlmMap subjectAlmMap;

      // The map where unique almanacs collected from a given satellite are stored.
      // The SatID is the identification of the TRANSMITTING satellite.
      typedef std::map<SatID, OrbAlmMap> UniqueAlmMap;
      typedef std::map<SatID, UniqueAlmMap> XmitAlmMap;
      XmitAlmMap xmitAlmMap;

      CommonTime initialTime; //< Time of the first OrbAlm
      CommonTime finalTime;   //< Time of the last OrbAlm

         // This contains state information regadrding the 
         // WNa/Toa for GPS LNAV and BeiDou.
      OrbAlmFactory orbAlmFactory;

         // Default constructor yields a specific invalid object. 
         // We'll use that to detect a default argument in find( ) method.
      SatID invalidSatID; 

      // Here are a couple of methods to simplify the .cpp
      void updateInitialFinal(const OrbAlm* alm)
      {
        if (alm->beginValid<initialTime)       
          initialTime = alm->beginValid;
         
        if (alm->endValid>finalTime)               
          finalTime = alm->endValid;
      }
      
     // virtual void dumpOnePRN( std::ostream& s = std::cout, OrbElemMap& em) const
     //    throw();

   }; // end class

   //@}

} // namespace

#endif
