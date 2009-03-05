#pragma ident "$Id"

/**
 * @file GloEphemerisStore.hpp
 * Get GLONASS ephemeris data information
 */


#ifndef GPSTK_GLO_EPHEMERIS_STORE_HPP
#define GPSTK_GLO_EPHEMERIS_STORE_HPP

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

#include "TabularEphemerisStore.hpp"
#include "GloRecord.hpp"

namespace gpstk
{

      /** @addtogroup ephemstore */
      //@{

      /**
       * This adds the interface to get Glonass Ephemeris Information
       */
   class GloEphemerisStore : public TabularEphemerisStore<GloRecord>
   {

   public:

         /// Constructor.
      GloEphemerisStore()
         throw()
         : rejectBadPosFlag(true), rejectBadClockFlag(true)
      { TabularEphemerisStore<GloRecord>(); };

         /// Destructor.
      virtual ~GloEphemerisStore() {};

      GloRecord getNearGloRecord(SatID sat, CommonTime t)
         throw (gpstk::InvalidRequest);

      GloRecord getFirstGloRecord(SatID sat)
         throw (gpstk::InvalidRequest)
      {return getNearGloRecord(sat, CommonTime::BEGINNING_OF_TIME);};
      
      GloRecord getLastGloRecord(SatID sat)
         throw (gpstk::InvalidRequest)
      {return getNearGloRecord(sat, CommonTime::END_OF_TIME);};

   };

}  // End of namespace gpstk

#endif   // GPSTK_GLO_EPHEMERIS_STORE_HPP
