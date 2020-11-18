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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/// @file ObsID.hpp
/// gpstk::ObsID - navigation system, receiver, and file specification
/// independent representation of the types of observation data that can
/// be collected.  This class is analogous to the RinexObsType class that
/// is used to represent the observation codes in a RINEX file. It is
/// intended to support at least everything in section 5.1 of the RINEX 3
/// specifications.

#ifndef OBSID_HPP
#define OBSID_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>

#include "Exception.hpp"
#include "SatID.hpp"
#include "Rinex3ObsBase.hpp"
#include "ObservationType.hpp"
#include "CarrierBand.hpp"
#include "TrackingCode.hpp"

namespace gpstk
{
   class ObsID;
   namespace StringUtils
   {
         /// convert this object to a string representation
      std::string asString(const ObsID& id);
   }


      /// stream output for ObsID
   std::ostream& operator<<(std::ostream& s, const ObsID& p);


   class ObsID
   {
   public:
         /// empty constructor, creates a wildcard object.
      ObsID()
            : type(ObservationType::Unknown), band(CarrierBand::Unknown),
              code(TrackingCode::Unknown)
      {}

         /// Explicit constructor
      ObsID(ObservationType ot, CarrierBand cb, TrackingCode tc)
            : type(ot), band(cb), code(tc)
      {}

         /// Equality requires all fields to be the same
      virtual bool operator==(const ObsID& right) const;

         /** This ordering is somewhat arbitrary but is required to be
          * able to use an ObsID as an index to a std::map. If an
          * application needs some other ordering, inherit and
          * override this function. One 'feature' that has been added
          * is that an Any code/carrier/type will match any other
          * code/carrier/type in the equality operator. The intent is
          * to support performing an operation like "tell me if this
          * is a pseudorange that was collected on L1 from *any*
          * code". */
      virtual bool operator<(const ObsID& right) const;

      bool operator!=(const ObsID& right) const
      { return !(operator==(right)); };

      bool operator>(const ObsID& right) const
      {  return (!operator<(right) && !operator==(right)); };

      bool operator<=(const ObsID& right) const
      { return (operator<(right) || operator==(right)); };

      bool operator>=(const ObsID& right) const
      { return !(operator<(right)); };

         /// Convenience output method
      virtual std::ostream& dump(std::ostream& s) const;

         /// Destructor
      virtual ~ObsID()
      {}

         // Note that these are the only data members of objects of this class.
      ObservationType  type;
      CarrierBand      band;
      TrackingCode     code;

         /// These strings are for forming a somewhat verbose description
      static std::map< TrackingCode,    std::string > tcDesc;
      static std::map< CarrierBand,     std::string > cbDesc;
      static std::map< ObservationType, std::string > otDesc;
   }; // class ObsID


} // namespace gpstk
#endif   // OBSID_HPP
