//============================================================================
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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/// @file Base64Encoder.hpp Used to encode values into Base64.  Class
/// declarations.

#ifndef VDRAW_BASE64ENCODER_H
#define VDRAW_BASE64ENCODER_H

#include<string>
#include<sstream>

#include "VDrawException.hpp"

namespace vdraw
{
  /// @ingroup BasicVectorGraphics  
  //@{

  /**
   * This is used to encode a string in base64 
   */
  class Base64Encoder 
  {

    public:
      /// Base64 Encoding (0-255)
      static const std::string encode_string;

      /// Encode the lowest 6 bits
      static char encode6(long s) { return encode_string[s&0x3F]; }      

      /// Encode the given string in base64 
      static std::string encode(const std::string &str);


  }; // class Base64Encoder

  //@}

} // namespace vdraw

#endif //VDRAW_BASE64ENCODER_H
