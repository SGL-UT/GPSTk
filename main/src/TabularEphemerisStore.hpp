#pragma ident "$Id: //depot/sgl/gpstk/dev/src/TabularEphemerisStore.hpp#3 $"

/**
 * @file TabularEphemerisStore.hpp
 * Store & access a list of SV pvts
 */

#ifndef GPSTK_TABULAR_EPHEMERIS_STORE_HPP
#define GPSTK_TABULAR_EPHEMERIS_STORE_HPP

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






#include <iostream>

#include "EphemerisStore.hpp"
#include "SP3Data.hpp"

namespace gpstk
{
   /** @addtogroup ephemstore */
   //@{

   /**
    * This stores tabular ephemeris data for determining satellite positions.
    */
   class TabularEphemerisStore : public EphemerisStore
   {
   public:
      /// Constructor.
      TabularEphemerisStore() throw()
         : haveVelocity(true),
           initialTime(DayTime::END_OF_TIME), 
           finalTime(DayTime::BEGINNING_OF_TIME) {};

      /// destructor
      virtual ~TabularEphemerisStore() {}
      
      /**  Return the position, velocity and clock model of the sv in ecef coordinates
       * (units m, s, m/s, s/s) at the indicated time.
       * @param prn the SV's PRN
       * @param t the time to look up
       * @return the Xvt of the SV at time t
       */
      virtual Xvt getPrnXvt(short prn, const gpstk::DayTime& t)
         const throw(EphemerisStore::NoEphemerisFound);

      /** Dump the store to given stream.
       * @param detail specifies the level of detail to include in the output:
       *   0: number of satellites, 1: records per satellite, >1 all data.
       */
      void dump(short detail=0, std::ostream& s=std::cout) const;

      /// Edit the dataset, removing data outside this time interval
      void edit(const DayTime& tmin, const DayTime& tmax) throw();

      /// Insert a new SP3Data object into the store
      void addEphemeris(const SP3Data& data)
         throw();

      /// Remove all data
      void clear() throw();

      /** Return the time of the first ephemeris in the object.
       * @return the time of the first ephemeris in the object
       */      
      virtual gpstk::DayTime getInitialTime() const {return initialTime;};

      /** Return the time of the last ephemeris in the object.
       * @return the time of the last ephemeris in the object
       */
      virtual gpstk::DayTime getFinalTime() const {return finalTime;};

      void setHaveVelocity(bool f) throw() {haveVelocity=f;};
      bool getHaveVelocity() throw() {return haveVelocity;};

   private:

      /// The key to this map is the time
      typedef std::map<DayTime, Xvt> SvEphMap;

      /// The key to this map is the svid of the satellite (usually the prn)
      typedef std::map<short, SvEphMap> EphMap;

      /// the map of SVs and XVTs
      EphMap pe;

      /** These give the overall span of time for which this object contains data.
       * NB there may be gaps in the data, i.e. the data may not be continuous.
       */
      gpstk::DayTime initialTime, finalTime;

      /// Flag indicating that velocity data present in all datasets loaded.
      bool haveVelocity;

   };

   //@}

}  // namespace

#endif
