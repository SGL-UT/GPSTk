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

#ifndef GPSTK_CARRIERBAND_HPP
#define GPSTK_CARRIERBAND_HPP

#include <string>
#include "EnumIterator.hpp"

namespace gpstk
{
   enum class CarrierBand
   {
      Unknown,   ///< Uninitialized value
      Any,       ///< Used to match any carrier band
      L1L2,      ///< GPS L1+L2
      L1,        ///< GPS L1, Galileo E1, SBAS L1, QZSS L1, BeiDou L1
      L2,        ///< GPS L2, QZSS L2
      L5,        ///< GPS L5, Galileo E5a, SBAS L5, QZSS L5, BeiDou B2a, NavIC L5
      G1,        ///< GLONASS G1
      G1a,       ///< GLONASS G1a
      G2a,       ///< GLONASS G2a
      G2,        ///< GLONASS G2
      G3,        ///< GLONASS G3
      E5b,       ///< Galileo E5b
      E5ab,      ///< Galileo E5, BeiDou B2
      E6,        ///< Galileo E6, QZSS L6
      B1,        ///< BeiDou B1
      B2,        ///< BeiDou B2b
      B3,        ///< BeiDou B3
      I9,        ///< NavIC S
      Undefined, ///< Code is known to be undefined (as opposed to unknown)
      Last,      ///< Used to verify that all items are described at compile time
   }; // enum class CarrierBand

      /** Define an iterator so C++11 can do things like
       * for (CarrierBand i : CarrierBandIterator()) */
   typedef EnumIterator<CarrierBand, CarrierBand::Unknown, CarrierBand::Last> CarrierBandIterator;

   namespace StringUtils
   {
         /// Convert a CarrierBand to a whitespace-free string name.
      std::string asString(CarrierBand e) throw();
         /// Convert a string name to an CarrierBand
      CarrierBand asCarrierBand(const std::string& s) throw();
   }
} // namespace gpstk

#endif // GPSTK_CARRIERBAND_HPP
