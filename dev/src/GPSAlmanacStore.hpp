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
 * @file GPSAlmanacStore.hpp
 * Store GPS almanac information (i.e. like the data in subframes 4&5) and 
 * compute satellite Xvt based upon this data and the algorithms defined
 * in the IS-GPS-200.
 */
 
#ifndef GPSTK_GPSALMANACSTORE_HPP
#define GPSTK_GPSALMANACSTORE_HPP

#include <iostream>
#include <string>
#include <map>

#include "XvtStore.hpp"
#include "SatID.hpp"
#include "AlmOrbit.hpp"
#include "EngAlmanac.hpp"

namespace gpstk
{
   /** @defgroup ephemstore */
   //@{

   /// Store GPS almanac information (i.e. like the data in subframes 4&5) and 
   /// compute satellite Xvt based upon this data and the algorithms defined
   /// in the IS-GPS-200.
   class GPSAlmanacStore : public XvtStore<SatID>
   {
   public:

      GPSAlmanacStore()
         throw()
         : initialTime(DayTime::END_OF_TIME), 
           finalTime(DayTime::BEGINNING_OF_TIME)
      {}

      virtual ~GPSAlmanacStore()
      {}
      
      /// Returns the position, velocity, and clock offset of the indicated
      /// object in ECEF coordinates (meters) at the indicated time.
      /// @param[in] id the object's identifier
      /// @param[in] t the time to look up
      /// @return the Xvt of the object at the indicated time
      /// @throw InvalidRequest If the request can not be completed for any
      ///    reason, this is thrown. The text may have additional
      ///    information as to why the request failed.
      virtual Xvt getXvt(const SatID id, const DayTime& t) 
         const throw(InvalidRequest);
      

      /// A debugging function that outputs in human readable form,
      /// all data stored in this object.
      /// @param[in] s the stream to receive the output; defaults to cout
      /// @param[in] detail the level of detail to provide
      virtual void dump(std::ostream& s = std::cout, short detail = 0) 
         const throw();


      /// Edit the dataset, removing data outside the indicated time interval
      /// @param[in] tmin defines the beginning of the time interval
      /// @param[in] tmax defines the end of the time interval
      virtual void edit(const DayTime& tmin, 
                        const DayTime& tmax = DayTime(DayTime::END_OF_TIME))
         throw();


      /// Determine the earliest time for which this object can successfully 
      /// determine the Xvt for any object.
      /// @return The initial time
      /// @throw InvalidRequest This is thrown if the object has no data.
      virtual DayTime getInitialTime()
         const throw(InvalidRequest)
      {return initialTime;}

      
      /// Determine the latest time for which this object can successfully 
      /// determine the Xvt for any object.
      /// @return The final time
      /// @throw InvalidRequest This is thrown if the object has no data.
      virtual DayTime getFinalTime()
         const throw(InvalidRequest)
      {return finalTime;}

      virtual bool velocityIsPresent()
         const throw()
      {return true;}

      
      virtual bool clockIsPresent()
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
      /// @throw InvalidRequest no data found in store
      short getSatHealth(SatID sat, const DayTime& t) 
         const throw(InvalidRequest);

      bool addAlmanac(const AlmOrbit& alm) throw();
      bool addAlmanac(const EngAlmanac& alm) throw();

      /// gets the closest almanac for the given time and satellite id,
      /// closest being in the past or future.
      /// @param sat the satellite's SatID
      /// @param t the time of interest
      AlmOrbit findAlmanac(SatID sat, const DayTime& t) 
         const throw(InvalidRequest);

      /// returns all almanacs closest to t for all satellites
      AlmOrbits findAlmanacs(const DayTime& t) 
         const throw(InvalidRequest);

   protected:
      /// This is intended to just store weekly sets of unique EngAlmanacs
      /// for a single SV.  The key is ToA
      typedef std::map<DayTime, AlmOrbit> EngAlmMap;

      /// This is intended to hold all unique EngEphemerises for each SV
      /// The key is the SatID of the SV.
      typedef std::map<SatID, EngAlmMap> UBAMap;

      /// The map where all EngAlmanacs are stored.
      UBAMap uba;

      DayTime initialTime; //< Earliest Toa minus a half week
      DayTime finalTime;   //< Last Toa plus a half week
      
   };

   //@}
}
#endif
