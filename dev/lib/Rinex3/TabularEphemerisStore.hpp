#pragma ident "$Id: TabularEphemerisStore.hpp 827 2007-10-10 17:41:45Z architest $"

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
 * @file TabularEphemerisStore.hpp
 * Store a tabular list of Xvt data (such as a table of precise ephemeris data
 * in an SP3 file) and compute Xvt from this table. A Lagrange interpolation
 * is used to compute the Xvt for times that are not in the table but do have
 * sufficient data.
 */

#ifndef GPSTK_TABULAR_EPHEMERIS_STORE_HPP
#define GPSTK_TABULAR_EPHEMERIS_STORE_HPP

#include <iostream>

#include "SatID.hpp"
#include "DayTime.hpp"
#include "XvtStore.hpp"
#include "SP3Data.hpp"

namespace gpstk
{
   /** @addtogroup ephemstore */
   //@{

   /// Store a tabular list of Xvt data (such as a table of precise ephemeris data
   /// in an SP3 file) and compute Xvt from this table. A Lagrange interpolation
   /// is used to compute the Xvt for times that are not in the table but do have
   /// sufficient data.
   class TabularEphemerisStore : public XvtStore<SatID>
   {
   public:
      TabularEphemerisStore()
         throw()
         : initialTime(DayTime::END_OF_TIME), 
           finalTime(DayTime::BEGINNING_OF_TIME)
           
      {}

      virtual ~TabularEphemerisStore()
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
      {return haveVelocity;}

      virtual bool clockIsPresent()
         const throw()
      {return true;}


      //---------------------------------------------------------------
      // Below are interfaces that are unique to this class (i.e. not 
      // in the parent class)
      //---------------------------------------------------------------

      /// Insert a new SP3Data object into the store
      void addEphemeris(const SP3Data& data)
         throw();

      /// Remove all data
      void clear() throw();


   protected:

      /// Flag indicating that velocity data present in all datasets loaded.
      bool haveVelocity;


   private:

      /// The key to this map is the time
      typedef std::map<DayTime, Xvt> SvEphMap;

      /// The key to this map is the svid of the satellite (usually the prn)
      typedef std::map<SatID, SvEphMap> EphMap;

      /// the map of SVs and XVTs
      EphMap pe;

      /** These give the overall span of time for which this object contains data.
       * NB there may be gaps in the data, i.e. the data may not be continuous.
       */
      DayTime initialTime, finalTime;

   };

   //@}

}  // namespace

#endif
