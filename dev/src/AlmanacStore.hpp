#pragma ident "$Id$"



#ifndef GPSTK_ALMANACSTORE_HPP
#define GPSTK_ALMANACSTORE_HPP

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
 * @file AlmanacStore.hpp
 * Store almanac information, and access by satellite and time
 */
 
#include <iostream>
#include <string>
#include <map>

#include "Exception.hpp"
#include "DayTime.hpp"
#include "AlmOrbit.hpp"
#include "EngAlmanac.hpp"

namespace gpstk
{
   /** @defgroup ephemstore Ephemeris I/O and Storage */
   //@{

      ///  This class defines an interface to hide how we are getting an SV's 
      /// position at some point in time using almanac data.
   class AlmanacStore
   {
   public:
         /// Thrown when attempting to read an almanac that isn't stored.
         /// @ingroup exceptiongroup
      NEW_EXCEPTION_CLASS(NoAlmanacFound, gpstk::Exception);

      Xvt getPrnXvt(short prn, const gpstk::DayTime& t)
         const throw(NoAlmanacFound);

      short getPrnHealth(short prn, const gpstk::DayTime& t)
         const throw(NoAlmanacFound);

      bool addAlmanac(const AlmOrbit& alm) throw();
      bool addAlmanac(const EngAlmanac& alm) throw();

         /// gets the closest almanac for the given time and prn,
         /// closest being in the past or future.
      AlmOrbit findAlmanac(short prn, const gpstk::DayTime& t) 
         const throw(NoAlmanacFound);

         /// returns all almanacs closest to t for all prns
      AlmOrbits findAlmanacs(const gpstk::DayTime& t) const;

         /// returns earliest Toa found in the set
      gpstk::DayTime getInitialTime() const;
      
         /// this isn't accurate, but the 0 case is all that's really important
      size_t size() const { return uba.size(); }

   protected:
         /** This is intended to just store weekly sets of unique EngAlmanacs
          * for a single SV.  The key is ToA
          */
      typedef std::map<gpstk::DayTime, AlmOrbit> EngAlmMap;

         /** This is intended to hold all unique EngEphemerises for each SV
          * The key is the prn of the SV.
          */
      typedef std::map<short, EngAlmMap> UBAMap;

         /// The map where all EngAlmanacs are stored.
      UBAMap uba;
   };

   //@}

}

#endif
