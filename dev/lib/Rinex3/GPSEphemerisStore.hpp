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
 * @file GPSEphemerisStore.hpp
 * Store GPS broadcast ephemeris information (i.e. like the data in
 * subframes 1-3) and computes satellite Xvt based upon this data and the
 * algorithms defined for that data in the IS-GPS-200.
 */
 
#ifndef GPSTK_GPSEPHEMERISSTORE_HPP
#define GPSTK_GPSEPHEMERISSTORE_HPP

#include <iostream>
#include <string>
#include <list>
#include <map>

#include "XvtStore.hpp"
#include "SatID.hpp"
#include "EngEphemeris.hpp"
#include "icd_200_constants.hpp"

namespace gpstk
{
   /** @addtogroup ephemstore */
   //@{
   
   /// Store GPS broadcast ephemeris information (i.e. like the data in
   /// subframes 1-3) and computes satellite Xvt based upon this data and the
   /// algorithms defined for that data in the IS-GPS-200.
   class GPSEphemerisStore : public KepEphemerisStore<SatID>
   {
   public:

      GPSEphemerisStore()
         throw()
         : initialTime(DayTime::END_OF_TIME), 
           finalTime(DayTime::BEGINNING_OF_TIME),
           method(0)
      {}


      virtual ~GPSEphemerisStore()
      {}


      /// Returns the position, velocity, and clock offset of the indicated
      /// satellite in ECEF coordinates (meters) at the indicated time.
      /// @param[in] sat the SV's SatID
      /// @param[in] t the time to look up
      /// @return the Xvt of the SV at time
      /// @throw InvalidRequest If the request can not be completed for any
      ///    reason, this is thrown. The text may have additional
      ///    information as to why the request failed.
      Xvt getXvt(const SatID sat, const DayTime& t)
         const throw(InvalidRequest);
      

      /// A debugging function that outputs in human readable form,
      /// all data stored in this object.
      /// @param[in] s the stream to receive the output; defaults to cout
      /// @param[in] detail the level of detail to provide
      void dump(std::ostream& s = std::cout, short detail = 0)
         const throw();


      /// Edit the dataset, removing data outside the indicated time interval
      /// @param tmin defines the beginning of the time interval
      /// @param tmax defines the end of the time interval
      void edit(const DayTime& tmin, 
                const DayTime& tmax = DayTime(DayTime::END_OF_TIME) )
         throw();


      /// Determine the earliest time for which this object can successfully 
      /// determine the Xvt for any satellite.
      /// @return The initial time
      /// @throw InvalidRequest This is thrown if the object has no data.
      DayTime getInitialTime()
         const throw(InvalidRequest)
      {return initialTime;}

      
      /// Determine the latest time for which this object can successfully 
      /// determine the Xvt for any satellite.
      /// @return The final time
      /// @throw InvalidRequest This is thrown if the object has no data.
      DayTime getFinalTime()
         const throw(InvalidRequest)
      {return finalTime;}


      bool velocityIsPresent()
         const throw()
      {return true;}


      bool clockIsPresent()
         const throw()
      {return true;}


      //---------------------------------------------------------------
      // Below are interfaces that are unique to this class (i.e. not 
      // in the parent class)
      //---------------------------------------------------------------

      /// Returns the health of an SV for a particular time
      /// @param sat the satellite's SatID
      /// @param t the time to look up
      /// @return the SV health bits
      /// @throw InvalidRequest no matching ephemeris found in the store
      short getSatHealth(const SatID sat, const DayTime& t)
         const throw(InvalidRequest);


      /// Add an EngEphemeris object to this collection.
      /// @param eph the EngEphemeris to add
      /// @return true if ephemeris was added, false otherwise
      bool addEphemeris(const EngEphemeris& eph)
         throw();

      
      /// Remove EngEphemeris objects older than t.
      /// @param t remove EngEphemeris objects older than this
      void wiper(const DayTime& t)
         throw()
      {edit(t);}
      
      /// Remove all data from this collection.   
      void clear()
         throw()
      {edit(DayTime(DayTime::END_OF_TIME));}
      
      /** This returns the pvt of the sv in ecef coordinates
       * (units m, s, m/s, s/s) at the indicated time.
       * @param sat the satellite's SatID
       * @param t the time to look up
       * @param ref a place to return the IODC for future reference.
       * @return the Xvt of the SV at time t
       */
      Xvt getXvt(const SatID sat, const DayTime& t, short& ref)
         const throw(InvalidRequest);

      /// Get the number of EngEphemeris objects in this collection.
      /// @return the number of EngEphemeris records in the map
      unsigned ubeSize()
         const throw();
      
      unsigned size()
         const throw()
      { return ubeSize(); };
      
      /// Find an ephemeris based upon the search method configured
      /// by SearchNear/SearchPast
      /// @param sat SatID of satellite of interest
      /// @param t time with which to search for ephemeris
      /// @return a reference to the desired ephemeris
      /// @throw InvalidRequest object thrown when no ephemeris is found
      const EngEphemeris& findEphemeris(const SatID sat, const DayTime& t)
         const throw(InvalidRequest);
      
      /// Find an ephemeris for the indicated satellite at time t. The ephemeris
      /// is chosen to be the one that 1) is within the fit interval
      /// for the given time of interest, and 2) is the last ephemeris
      /// transmitted before the time of interest (i.e. min(toi - HOW time)).
      /// @param sat the SV of interest
      /// @param t the time of interest
      /// @return a reference to the desired ephemeris
      /// @throw InvalidRequest object thrown when no ephemeris is found
      const EngEphemeris& findUserEphemeris(const SatID sat, const DayTime& t)
         const throw(InvalidRequest);

      /// Find an ephemeris for the indicated satellite at time t. The ephemeris
      /// chosen is the one with HOW time closest to the time t, (i.e. with
      /// smallest fabs(t-HOW), but still within the fit interval.
      /// @param sat the SV of interest
      /// @param t the time of interest
      /// @return a reference to desired ephemeris
      /// @throw InvalidRequest object thrown when no ephemeris is found
      const EngEphemeris& findNearEphemeris(const SatID sat, const DayTime& t)
         const throw(InvalidRequest);

      /// Add all ephemerides to an existing list<EngEphemeris>.
      /// @return the number of ephemerides added.
      int addToList(std::list<EngEphemeris>& v)
         const throw();

      /// use findNearEphemeris() in the getSat...() routines
      void SearchNear(void)
         throw()
      {method = 1;}

      /// use findEphemeris() in the getSat...() routines (the default)
      void SearchPast(void)
         throw()
      {method = 0;}

      /// This is intended to just store sets of unique EngEphemerides
      /// for a single SV.  The key is the Toe - 1/2 the fit interval.
      typedef std::map<DayTime, EngEphemeris> EngEphMap;
      
      /// Returns a map of the ephemerides available for the specified
      /// satellite.  Note that the return is specifically chosen as a 
      /// const reference.  The intent is to provide "read only" access
      /// for analysis.  If the map needs to be modified, see other methods.
      const EngEphMap& getEphMap( const SatID sat )
               const throw(InvalidRequest);

   private:
      
      /// This is intended to hold all unique EngEphemerides for each SV
      /// The key is the prn of the SV.
      typedef std::map<short, EngEphMap> UBEMap;
      
      /// The map where all EngEphemerides are stored.
      UBEMap ube;
      
      DayTime initialTime; //< Time of the first EngEphemeris
      DayTime finalTime;   //< Time of the last EngEphemeris
      
      /// flag indicating search method (find...Eph) to use in getSatXvt 
      ///  and getSatHealth
      int method;
      
   }; // end class GPSEphemerisStore
   //@}
   
} // namespace gpstk
#endif  // GPSTK_GPSEPHEMERISSTORE_HPP
