//==============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#ifndef NAVFILTERKEY_HPP
#define NAVFILTERKEY_HPP

#include <ostream>
#include "ObsID.hpp"
#include "CommonTime.hpp"
#include "gpstkplatform.h"

namespace gpstk
{
      /// @ingroup NavFilter
      //@{

      /** Defines a class that is used by NavFilter and child classes
       * to both identify the source of a navigation message as well
       * as the message itself.  The navigation messages are defined
       * as child classes of NavFilterKey such that the message-format
       * specific data is defined.
       *
       * To put it another way, this is the base class and the child
       * classes define the data storage for the nav message.
       *
       * @note The data members in this class are not always required
       *   to be set.  The use of the data field members is
       *   filter-dependent.  Consult the preconditions of the
       *   validate method of the filters in use to determine which
       *   data members MUST be set. */
   class NavFilterKey
   {
   public:
         /// Initialize key members to empty defaults
      NavFilterKey();
         /** Time stamp for the nav subframe.  This may vary depending
          * on GNSS and implementation.  Refer to the documentation
          * for the validate methods of individual filters for any
          * requirements on the contents of this field. */
      gpstk::CommonTime timeStamp;
      std::string stationID;       ///< site/station identifier for data source
      std::string rxID;            ///< receiver identifier for data source
      uint32_t prn;                ///< identifier of broadcasting satellite 
      ObsID::CarrierBand carrier;  ///< carrier band of navigation message
      ObsID::TrackingCode code;    ///< ranging code of navigation message
         // Nav code is not necessary as each filter is unique to a
         // given navigation message structure.

      virtual void dump(std::ostream& s) const; 

         // Do not define an operator<(), let the filter classes
         // define their own sorting algorithms as needed.
   protected:
         // make this a polymorphic type so dynamic_cast works
      virtual void dummy() {}

   };

      //@}
   
      // Write to output stream
   std::ostream& operator<<(std::ostream& s, const NavFilterKey& nfk); 
}

#endif // NAVFILTERKEY_HPP
