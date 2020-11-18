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

#include "ReferenceFrame.hpp"

using namespace std;

namespace gpstk
{
   namespace StringUtils
   {
      std::string asString(ReferenceFrame e)
      {
         switch (e)
         {
            case ReferenceFrame::Unknown:    return "Unknown";
            case ReferenceFrame::WGS84:      return "WGS84";
            case ReferenceFrame::WGS84G730:  return "WGS84(G730)";
            case ReferenceFrame::WGS84G873:  return "WGS84(G873)";
            case ReferenceFrame::WGS84G1150: return "WGS84(G1150)";
            case ReferenceFrame::ITRF:       return "ITRF";
            case ReferenceFrame::PZ90:       return "PZ90";
            case ReferenceFrame::PZ90KGS:    return "PZ90KGS";
            case ReferenceFrame::CGCS2000:   return "CGCS2000";
            default:                         return "???";
         }
      }


      ReferenceFrame asReferenceFrame(const std::string& s)
      {
         if (s == "Unknown")      return ReferenceFrame::Unknown;
         if (s == "WGS84")        return ReferenceFrame::WGS84;
         if (s == "WGS84(G730)")  return ReferenceFrame::WGS84G730;
         if (s == "WGS84(G873)")  return ReferenceFrame::WGS84G873;
         if (s == "WGS84(G1150)") return ReferenceFrame::WGS84G1150;
         if (s == "ITRF")         return ReferenceFrame::ITRF;
         if (s == "PZ90")         return ReferenceFrame::PZ90;
         if (s == "PZ90KGS")      return ReferenceFrame::PZ90KGS;
         if (s == "CGCS2000")     return ReferenceFrame::CGCS2000;
         return ReferenceFrame::Unknown;
      }
   }
}   // end namespace
