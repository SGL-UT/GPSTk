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

#include <NavMsgDataPNB.hpp>

namespace gpstk
{
   uint32_t NavMsgDataPNB ::
   getBits(unsigned start, unsigned num) const
   {
      return sf->asUnsignedLong(start-1, num, 1);
   }


   void NavMsgDataPNB ::
   dump(std::ostream& s, unsigned totalBits) const
   {
         /** @todo eventually we'll likely want to make this a bit
          * more configurable.  This applies to GPS and BeiDou at the
          * very least, but probably not GLONASS.  Configuration
          * shouldn't involve the use of variables in this class if it
          * can be avoided, since there can be a large number of these
          * objects when processing data.  It would be better to
          * figure out a way to make the nav code-specific derived
          * classes set the configuration somehow via methods or
          * static data or some such. */
      sf->outputPackedBits(s, 1000, ' ', 30);
   }
} // namespace gpstk
