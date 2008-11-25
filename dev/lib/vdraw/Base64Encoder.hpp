#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/Base64Encoder.hpp#1 $"

/// @file Base64Encoder.hpp Used to encode values into Base64.  Class
/// declarations.

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================


#ifndef VDRAW_BASE64ENCODER_H
#define VDRAW_BASE64ENCODER_H

#include<string>
#include<sstream>

#include "VDrawException.hpp"

namespace vdraw
{
  /** \addtogroup BasicVectorGraphics */ 
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

