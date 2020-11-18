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
 * @file Rinex3ClockBase.hpp
 * Base class for Rinex3Clock file data
 */

#ifndef RINEX3CLOCKBASE_HPP
#define RINEX3CLOCKBASE_HPP

#include "FFData.hpp"
#include "FormattedDouble.hpp"

namespace gpstk
{
      /// @ingroup FileHandling
      //@{

      /// This class is here to make readable inheritance diagrams.
   class Rinex3ClockBase : public FFData
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

         /// Destructor per the coding standards
      virtual ~Rinex3ClockBase() {}
   };

      //@}

}  // namespace

#endif   // RINEX3CLOCKBASE_HPP
