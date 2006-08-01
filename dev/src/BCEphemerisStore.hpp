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

/**
 * @file BCEphemerisStore.hpp
 * Store GPS broadcast ephemeris information, and access by satellite and time
 */
 
#ifndef GPSTK_BCEPHEMERISSTORE_HPP
#define GPSTK_BCEPHEMERISSTORE_HPP


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
#include <string>
#include <list>
#include <map>

#include "EphemerisStore.hpp"
#include "EngEphemeris.hpp"

namespace gpstk
{
      /** @addtogroup ephemstore */
      //@{
   
      /// Broadcast Ephemeris Store.  In this variation of the EphemerisStore,
      ///  all ephemerides are broadcast from a satellite.
   class BCEphemerisStore : public EphemerisStore
   {
   public:
      
         /// Constructor.
      BCEphemerisStore() 
         throw()
            : initialTime(DayTime::END_OF_TIME), 
              finalTime(DayTime::BEGINNING_OF_TIME),
              method(0) {};

         /// Destructor.
      virtual ~BCEphemerisStore() {}
   
         /**  This returns the pvt of the sv in ecef coordinates
          * (units m, s, m/s, s/s) at the indicated time.
          * @param prn the SV's PRN
          * @param t the time to look up
          * @return the Xvt of the SV at time t
          */
      virtual Xvt getPrnXvt(short prn, 
                            const gpstk::DayTime& t) const 
         throw(NoEphemerisFound);
      
         /** Return the health of an SV for a particular time
          * @param prn the SV's PRN
          * @param t the time to look up
          * @return the SV health bits
          * @throw NoEphemerisFound no matching ephemeris found in the store
          */
      virtual short getPrnHealth(short prn,
                                 const gpstk::DayTime& t) const 
         throw(NoEphemerisFound);
      
         /** Dumps all the ephemeris data stored in this object.
          * @param detail the level of detail to provide
          */
      void dump(short detail = 0, 
                std::ostream& s = std::cout) const ;
      
         /** Return the time of the first ephemeris in the object.
          * @return the time of the first ephemeris in the object
          */      
      virtual gpstk::DayTime getInitialTime() const
      { return initialTime; }
      
         /** Return the time of the last ephemeris in the object.
          * @return the time of the last ephemeris in the object
          */
      virtual gpstk::DayTime getFinalTime() const 
      { return finalTime; }
      
         /** Add an EngEphemeris object to this collection.
          * @param eph the EngEphemeris to add
          * @return true if ephemeris was added, false otherwise
          */
      bool addEphemeris(const EngEphemeris& eph)
         throw(gpstk::InvalidRequest);
      
         /** Remove EngEphemeris objects older than t.
          * @param t remove EngEphemeris objects older than this
          * @return number of Engephemeris objects removed
          */
      unsigned wiper(const DayTime& t)
         throw(gpstk::InvalidRequest);
      
         /// Edit the dataset, removing data outside this time interval
      virtual void edit(const DayTime& tmin,
                        const DayTime& tmax) ;
      
         /// Remove all data from this collection.   
      void clear() throw();
      
      
         //---------------------------------------------------------------
         // Below are interfaces that are unique to this class (i.e. not 
         // in the parent class)
         //---------------------------------------------------------------
      
         /**  This returns the pvt of the sv in ecef coordinates
          * (units m, s, m/s, s/s) at the indicated time.
          * @param prn the SV's PRN
          * @param t the time to look up
          * @param ref a place to return the IODC for future reference.
          * @return the Xvt of the SV at time t
          */
      Xvt getPrnXvt(short prn,
                    const gpstk::DayTime& t,
                    short& ref) const
         throw(NoEphemerisFound);
      
         /** Get the number of EngEphemeris objects in this collection.
          * @return the number of EngEphemeris records in the map
          */
      unsigned ubeSize() const 
         throw();
      
      unsigned size() const
      { return ubeSize(); };
      
         /** Find an ephemeris based upon the search method configured
          * by SearchNear/SearchPast
          */
      const EngEphemeris& findEphemeris(short prn, 
                                        const gpstk::DayTime& t) const 
         throw(NoEphemerisFound);
      
         /** Find the EngEphemeris for prn at time t.  The ephemeris
          * is chosen to be the one that 1) is within the fit interval
          * for the given time of interest, and 2) is the last
          * ephemeris transmitted before the time of interest
          * (i.e. min(toi - HOW time)).
          *
          * @param prn the SV's PRN of interest
          * @param t the time of interest
          * @return a reference to the EngEphemeris for prn at time t
          */
      const EngEphemeris& findUserEphemeris(short prn,
                                            const gpstk::DayTime& t) const 
         throw(NoEphemerisFound);
      
         /** Find the EngEphemeris for satellite prn at time t. The
          * ephemeris chosen is the one with HOW time closest to the
          * time t, i.e. with smallest fabs(t-HOW), but still within
          * the fit interval.
          *
          * @param prn the SV's PRN of interest
          * @param t the time of interest
          * @return a reference to the EngEphemeris for prn at time t
          */
      const EngEphemeris& findNearEphemeris(short prn, 
                                            const gpstk::DayTime& t) const
         throw(NoEphemerisFound);
      
         /** Add all ephemerides to an existing list<EngEphemeris>.
          * Return the number of ephemerides added.
          */
      int addToList(std::list<EngEphemeris>& v) const
         throw();
      
         /// use findNearEphemeris() in the getPrn...() routines
      void SearchNear(void) 
      { method = 1; }
      
         /// use findEphemeris() in the getPrn...() routines (the default)
      void SearchPast(void)
      { method = 0; }
      
   private:
         /** This is intended to just store weekly sets of unique EngEphemerides
          * for a single SV.  The key is the Toe - 1/2 the fit interval.
          */
      typedef std::map<gpstk::DayTime, EngEphemeris> EngEphMap;
      
         /** This is intended to hold all unique EngEphemerides for each SV
          * The key is the prn of the SV.
          */
      typedef std::map<short, EngEphMap> UBEMap;
      
         /// The map where all EngEphemerides are stored.
      UBEMap ube;
      
      gpstk::DayTime initialTime; //< Time of the first EngEphemeris
      gpstk::DayTime finalTime;   //< Time of the last EngEphemeris
      
         /// flag indicating search method (find...Eph) to use in getPrnXvt 
         ///  and getPrnHealth
      int method;
      
   }; // end class BCEphemerisStore
   
      //@}
   
} // namespace gpstk

#endif  // GPSTK_BCEPHEMERISSTORE_HPP
