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

/**
 * @file RinexClockBase.hpp
 * Base class for RINEX clock data file
 */

#ifndef GPSTK_RINEXCLOCKBASE_HPP
#define GPSTK_RINEXCLOCKBASE_HPP

#include <string>
#include "FFData.hpp"
#include "StringUtils.hpp"
#include "CivilTime.hpp"
#include "FormattedDouble.hpp"

namespace gpstk
{ 
      /// @ingroup FileHandling
      //@{

   class RinexClockBase : public FFData
   {
   public:  
      class RCDouble : public FormattedDouble
      {
      public:
            /// Constructor for a value, with all defaults for format.
         RCDouble(double d)
               : FormattedDouble(d, StringUtils::FFLead::Zero, 13, 2, 19, 'E',
                                 StringUtils::FFSign::NegOnly,
                                 StringUtils::FFAlign::Right)
         {}

            /// Assign a value by decoding a string using existing formatting.
         RCDouble& operator=(const std::string& s)
         { FormattedDouble::operator=(s); return *this; }
      };

         /// Destructor 
      virtual ~RinexClockBase() {}
      
         /// RINEX clock data types
      struct RinexClkType
      {
         std::string type;
         std::string description;
         RinexClkType() : type(std::string("UN")),
                          description(std::string("Unknown or Invalid")){}
         RinexClkType(std::string t, std::string d) : type(t), description(d){}
         bool operator==(const RinexClkType &other) const
         {  return (StringUtils::upperCase(type) == 
                    StringUtils::upperCase(other.type));
         }
         bool operator!=(const RinexClkType &other) const
         {  return !(*this == other); }
         bool operator<(const RinexClkType &other) const
         {  return (type < other.type); }
         bool operator<=(const RinexClkType &other) const
         {  return (type <= other.type); }
         bool operator>(const RinexClkType &other) const
         {  return (type > other.type); }
         bool operator>=(const RinexClkType &other) const
         {  return (type >= other.type); }
      };
      
         /** @name Standard RINEX clock data types
          */
         //@{
      static const RinexClkType UN;
      static const RinexClkType AR;
      static const RinexClkType AS;
      static const RinexClkType CR;
      static const RinexClkType DR;
      static const RinexClkType MS;    
         //@}

   protected:
      
         /// Converts a CivilTime object into a Rinex Clock time
         /// Format is 26 characters "yyyy mm dd hh mm ss.ssssss"
         /// If CommonTime == BEGINNING_OF_TIME an all blank string will
         /// be returned.
      std::string writeTime(const CivilTime& dt) const;
      
         /** Converts a 26 character Rinex Clock time in the format 
          * "yyyy mm dd hh mm ss.ssssss" to a CivilTime object.
          * If the string is blank a CommonTime::BEGINNING_OF_TIME is returned.
          * @throw FFStreamError
          */
      CivilTime parseTime(const std::string& line) const;
      
   };  //  RinexClockBase
      //@}
   
}  // namespace

#endif
