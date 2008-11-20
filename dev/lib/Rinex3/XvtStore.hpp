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
 * @file XvtStore.hpp
 * Abstract base class for storing and/or computing position, velocity, 
 * and clock data.
 */

#ifndef GPSTK_XVTSTORE_HPP
#define GPSTK_XVTSTORE_HPP

#include <iostream>
#include <string>
#include <list>
#include <map>

#include "Exception.hpp"
#include "SatID.hpp"
#include "DayTime.hpp"
#include "CommonTime.hpp"
#include "Xvt.hpp"

using namespace gpstk;

namespace Rinex3
{
   /** @addtogroup ephemstore */
   //@{

   /// Abstract base class for storing and accessing an object's position, 
   /// velocity, and clock data. Also defines a simple interface to remove
   /// data that had been added.
   template <class IndexType>
   class XvtStore
   {
   public:
      virtual ~XvtStore()
      {}
      
      /// Returns the position, velocity, and clock offset of the indicated
      /// object in ECEF coordinates (meters) at the indicated time.
      /// @param[in] id the object's identifier
      /// @param[in] t the time to look up
      /// @return the Xvt of the object at the indicated time
      /// @throw InvalidRequest If the request can not be completed for any
      ///    reason, this is thrown. The text may have additional
      ///    information as to why the request failed.
      virtual Xvt getXvt(const IndexType id, const CommonTime& t)
         const throw(InvalidRequest)
         = 0;

      /// A debugging function that outputs in human readable form,
      /// all data stored in this object.
      /// @param[in] s the stream to receive the output; defaults to cout
      /// @param[in] detail the level of detail to provide
      virtual void dump(std::ostream& s = std::cout, short detail = 0)
         const throw()
      {}

      /// Edit the dataset, removing data outside the indicated time interval
      /// @param[in] tmin defines the beginning of the time interval
      /// @param[in] tmax defines the end of the time interval
      virtual void edit(const CommonTime& tmin, 
                        const CommonTime& tmax = CommonTime(CommonTime::END_OF_TIME))
         throw()
         = 0;

      /// Determine the earliest time for which this object can successfully 
      /// determine the Xvt for any object.
      /// @return The initial time
      /// @throw InvalidRequest This is thrown if the object has no data.
      virtual CommonTime getInitialTime()
         const throw(InvalidRequest)
         = 0;

      /// Determine the latest time for which this object can successfully 
      /// determine the Xvt for any object.
      /// @return The final time
      /// @throw InvalidRequest This is thrown if the object has no data.
      virtual CommonTime getFinalTime()
         const throw(InvalidRequest)
         = 0;

      virtual bool velocityIsPresent()
         const throw()
         = 0;

      virtual bool clockIsPresent()
         const throw()
         = 0;
   }; // end class XvtStore

   //@}

} // namespace

#endif
