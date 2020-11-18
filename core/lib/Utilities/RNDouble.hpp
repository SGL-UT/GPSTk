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

#ifndef GPSTK_RNDOUBLE_HPP
#define GPSTK_RNDOUBLE_HPP

#include "FormattedDouble.hpp"

namespace gpstk
{
      /** This class specializes FormattedDouble to cover the data
       * that is present in RINEX NAV data files.  The formatting of
       * individual data is the same between RINEX 2 and 3 so we put
       * the shared implementation here. */
   class RNDouble : public FormattedDouble
   {
   public:
         /// Constructor for a value, with all defaults for format.
      RNDouble(double d = 0)
            : FormattedDouble(d, StringUtils::FFLead::Decimal, 12, 2, 19,
                              'D', StringUtils::FFSign::NegOnly,
                              StringUtils::FFAlign::Right)
      {}
         /// Decode a string.
      RNDouble(const std::string& str)
            : FormattedDouble(str, 19, 'D')
      {}

         /// Assign a value by decoding a string using existing formatting.
      RNDouble& operator=(const std::string& s)
      { FormattedDouble::operator=(s); return *this; }
   };
}

#endif // GPSTK_RNDOUBLE_HPP
