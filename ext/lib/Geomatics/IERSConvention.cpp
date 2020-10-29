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

/// @file IERSConvention.cpp
/// Implementation of class IERSConvention.

// GPSTk includes
#include "IERSConvention.hpp"

using namespace std;

namespace gpstk
{
   namespace StringUtils
   {
      std::string asString(IERSConvention e)
      {
         switch (e)
         {
            case IERSConvention::Unknown:  return "Unknown";
            case IERSConvention::IERS1996: return "IERS1996";
            case IERSConvention::IERS2003: return "IERS2003";
            case IERSConvention::IERS2010: return "IERS2010";
            default:                       return "???";
         }
      }


      IERSConvention asIERSConvention(const std::string& s)
      {
         if (s == "Unknown")  return IERSConvention::Unknown;
         if (s == "IERS1996") return IERSConvention::IERS1996;
         if (s == "IERS2003") return IERSConvention::IERS2003;
         if (s == "IERS2010") return IERSConvention::IERS2010;
         return IERSConvention::Unknown;
      }
   }
} // end namespace gpstk
