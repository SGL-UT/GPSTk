#pragma ident "$Id: $"

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
//  Copyright 2007, The University of Texas at Austin
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
 * @file MSCStore.hpp
 * Store Monitor Station coordinate information and return either the
 * raw information or the position at a given time based on integration 
 * of station velocity information over time since the epoch time. 
 */
 
#ifndef GPSTK_MSCSTORE_HPP
#define GPSTK_MSCSTORE_HPP

#include <iostream>
#include <string>
#include <list>
#include <map>

#include "FileStore.hpp"
#include "XvtStore.hpp"
#include "MSCData.hpp"
#include "MSCStream.hpp"
#include "MSCHeader.hpp"

namespace gpstk
{
   /** @addtogroup msc */
   //@{
   
   /// Store Monitor Station coordinate information and return either the
   /// raw information or the position at a given time based on integration 
   /// of station velocity information over time since the epoch time. 
   class MSCStore : public XvtStore<std::string>,
                    public FileStore<gpstk::MSCHeader>
   {
   public:

      MSCStore()
         throw()
         : initialTime(DayTime::END_OF_TIME), 
           finalTime(DayTime::END_OF_TIME)
      {}

      virtual ~MSCStore()
      {}

      /// Returns the position, velocity, and clock offset of the indicated
      /// satellite in ECEF coordinates (meters) at the indicated time.
      /// @param[in] sat the SV's SatID
      /// @param[in] t the time to look up
      /// @return the Xvt of the SV at time
      /// @throw InvalidRequest If the request can not be completed for any
      ///    reason, this is thrown. The text may have additional
      ///    information as to why the request failed.
      Xvt getXvt(const std::string stationID, const DayTime& t)
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
      void edit(const DayTime& tmin = DayTime(DayTime::BEGINNING_OF_TIME), 
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
      {return false;}

      //---------------------------------------------------------------
      // FileStore interfaces
      //---------------------------------------------------------------
      void loadFile(const std::string& filename) 
         throw(FileMissingException);	 

      //---------------------------------------------------------------
      // Below are interfaces that are unique to this class (i.e. not 
      // in the parent class)
      //---------------------------------------------------------------
      /// Add an EngEphemeris object to this collection.
      /// @param eph the EngEphemeris to add
      /// @return true if ephemeris was added, false otherwise
      bool addMSC(const MSCData& msc)
         throw();
      
      /// Remove all data from this collection.   
      void clear()
         throw()
      {edit(gpstk::DayTime(gpstk::DayTime::BEGINNING_OF_TIME), 
            gpstk::DayTime(gpstk::DayTime::BEGINNING_OF_TIME));}
      
      /// Get the number of MSCData objects in this collection.
      /// @return the number of MSCData records in the map
      unsigned size()
         const throw();
      
      /// Find an appropriate MSCData object for a given station and time.
      /// @param stationID ID of station of interest
      /// @param t time of interest
      /// @throw InvalidRequest object thrown when no ephemeris is found
      /// Note: There may be more than one MSCData object for a given
      /// station.  If so, findMSC( ) returns the MSCData object with the
      /// latest epoch time that is prior to t. 
      const MSCData& findMSC(const std::string stationID, const DayTime& t)
         const throw(InvalidRequest);
      const MSCData& findMSC(const unsigned long stationID, const DayTime& t) 
         const throw(InvalidRequest);

      /// Add all ephemerides to an existing list<EngEphemeris>.
      /// @return the number of ephemerides added.
      int addToList(std::list<MSCData>& v)
         const throw();

   private:
      /// StaMSCMap is a list of MSCData objects for a particular station
      /// in order of their effective epoch
      typedef std::map<gpstk::DayTime, MSCData> StaMSCMap;
      typedef StaMSCMap::const_iterator SMMci;
      typedef StaMSCMap::iterator SMMi;
      
      /// MSCMap is a set of StaMSCMap objects for all stations
      typedef std::map<std::string, StaMSCMap> MSCMap;
      typedef MSCMap::const_iterator MMci;
      typedef MSCMap::iterator MMi;
      
      /// The map where all EngEphemerides are stored.
      MSCMap mscMap;
      
      DayTime initialTime; //< Time of the first MSCData object
      DayTime finalTime;   //< Time of the last MSCData object
                           //< (N.B.: finalTime is irrelevant in the 
                           //<  current implementation as there is no
                           //<  "end of effectivity" for an MSCData object.
      
      static const double SEC_YEAR;
   }; // end class MSCStore
   //@}
   
} // namespace gpstk
#endif  // GPSTK_MSCSTORE_HPP
