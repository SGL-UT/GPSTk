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
 * @file EphemerisStore.hpp
 * Base for ephemeris storage classes
 */
 
#ifndef GPSTK_EPHEMERISSTORE_HPP
#define GPSTK_EPHEMERISSTORE_HPP

#include <iostream>
#include <string>
#include <list>
#include <map>

#include "Exception.hpp"
#include "SatID.hpp"
#include "DayTime.hpp"
#include "Xvt.hpp"

namespace gpstk
{
   /** @addtogroup ephemstore */
   //@{

      ///  This class defines an interface to hide how we are getting an SV's 
      /// position at some point in time.
   class EphemerisStore
   {
   public:
         /// Thrown when attempting to read an ephemeris that isn't stored.
         /// @ingroup exceptiongroup
      NEW_EXCEPTION_CLASS(NoEphemerisFound, gpstk::Exception);

         /// Thrown when attempting to read TGD from an ephemeris that doesn't have it.
         /// @ingroup exceptiongroup
      NEW_EXCEPTION_CLASS(NoTGDFound, gpstk::Exception);

         /// destructor.
      virtual ~EphemerisStore() {}
      
         /** This returns the PVT of the SV in ECEF coordinates at the 
          * indicated time.
          * @param sat the SV's SatID
          * @param t the time to look up
          * @return the Xvt of the SV at time t
          */
      virtual Xvt getSatXvt(SatID sat,
                            const gpstk::DayTime& t) const
         throw(NoEphemerisFound) = 0;

         /// \deprecated use the SatID version getSatXvt
      virtual Xvt getPrnXvt(short prn, 
                            const gpstk::DayTime& t) const
         throw(NoEphemerisFound) = 0;

         /** This method returns the Total Group Delay of the SV (in meters) at the 
          * indicated time.
          * @param sat    the SV's SatID
          * @param t      the time to look up
          * @return       the TGD (in meters) of the SV at time t
          */
      virtual double getTGD(SatID sat, const gpstk::DayTime& t) const
         throw(NoTGDFound) { return 0.0; }

         /** Dumps all the ephemeris data stored in this object.
          * @param detail the level of detail to provide
          */
      virtual void dump(short detail = 0,
                        std::ostream& s = std::cout) const = 0;

         /// Edit the dataset, removing data outside this time interval
      virtual void edit(const DayTime& tmin, 
                        const DayTime& tmax 
                        = DayTime(DayTime::END_OF_TIME) ) = 0;

         /** Return the time of the first ephemeris in the object.
          * @return the time of the first ephemeris in the object
          */
      virtual gpstk::DayTime getInitialTime() const = 0;
      
         /** Return the time of the last ephemeris in the object.
          * @return the time of the last ephemeris in the object
          */
      virtual gpstk::DayTime getFinalTime() const = 0;

   }; // end class EphemerisStore

   //@}

} // namespace

#endif
