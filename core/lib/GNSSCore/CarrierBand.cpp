/** @warning This code is automatically generated.
 *
 *  DO NOT EDIT THIS CODE BY HAND.
 *
 *  Refer to the documenation in the toolkit_docs gitlab project.
 */

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

#include "CarrierBand.hpp"

namespace gpstk
{
   namespace StringUtils
   {
      std::string asString(CarrierBand e) throw()
      {
         switch (e)
         {
            case CarrierBand::Unknown:   return "Unknown";
            case CarrierBand::Any:       return "Any";
            case CarrierBand::L1L2:      return "L1L2";
            case CarrierBand::L1:        return "L1";
            case CarrierBand::L2:        return "L2";
            case CarrierBand::L5:        return "L5";
            case CarrierBand::G1:        return "G1";
            case CarrierBand::G1a:       return "G1a";
            case CarrierBand::G2a:       return "G2a";
            case CarrierBand::G2:        return "G2";
            case CarrierBand::G3:        return "G3";
            case CarrierBand::E5b:       return "E5b";
            case CarrierBand::E5ab:      return "E5ab";
            case CarrierBand::E6:        return "E6";
            case CarrierBand::B1:        return "B1";
            case CarrierBand::B2:        return "B2";
            case CarrierBand::B3:        return "B3";
            case CarrierBand::I9:        return "I9";
            case CarrierBand::Undefined: return "Undefined";
            default:                     return "???";
         } // switch (e)
      } // asString(CarrierBand)


      CarrierBand asCarrierBand(const std::string& s) throw()
      {
         if (s == "Unknown")
            return CarrierBand::Unknown;
         if (s == "Any")
            return CarrierBand::Any;
         if (s == "L1L2")
            return CarrierBand::L1L2;
         if (s == "L1")
            return CarrierBand::L1;
         if (s == "L2")
            return CarrierBand::L2;
         if (s == "L5")
            return CarrierBand::L5;
         if (s == "G1")
            return CarrierBand::G1;
         if (s == "G1a")
            return CarrierBand::G1a;
         if (s == "G2a")
            return CarrierBand::G2a;
         if (s == "G2")
            return CarrierBand::G2;
         if (s == "G3")
            return CarrierBand::G3;
         if (s == "E5b")
            return CarrierBand::E5b;
         if (s == "E5ab")
            return CarrierBand::E5ab;
         if (s == "E6")
            return CarrierBand::E6;
         if (s == "B1")
            return CarrierBand::B1;
         if (s == "B2")
            return CarrierBand::B2;
         if (s == "B3")
            return CarrierBand::B3;
         if (s == "I9")
            return CarrierBand::I9;
         if (s == "Undefined")
            return CarrierBand::Undefined;
         return CarrierBand::Unknown;
      } // asCarrierBand(string)
   } // namespace StringUtils
} // namespace gpstk
