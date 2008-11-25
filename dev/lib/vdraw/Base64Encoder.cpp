#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/Base64Encoder.cpp#2 $"

/// @file Base64Encoder.cpp Used to encode values into Base64.  Class
/// definitions.

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

#include "Base64Encoder.hpp"

namespace vdraw
{
  const std::string Base64Encoder::encode_string = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  // The above should be equivalent to the one below but a good deal more
  // effecient as the size of the return string is determined up front and with
  // one allocation, the rest of the changes are indexed.
  // Keeping the old version around for now just in case.
#if 1
  std::string Base64Encoder::encode(const std::string &str)
  {
    int remainder = (int)str.size() % 3;        
    int end = (int)str.size() - remainder;
    std::string s((end/3+(remainder?1:0))*4,(char)0);
    int x = 0;
    long buffer;
    // Every 3 bytes can be encoded in 4x 6-bit characters
    for(int i=0;i<end;i+=3)
    {
      buffer = ((0x0FF&str[i])<<16) | ((0x0FF&str[i+1])<<8) | (0x0FF&str[i+2]);
      s[x++] = encode6(buffer>>18);
      s[x++] = encode6(buffer>>12);
      s[x++] = encode6(buffer>>6);
      s[x++] = encode6(buffer);
    }
    if(remainder>=1)
    {
      buffer = (0x0FF&str[end])<<16;
      if(remainder==2)
        buffer |= (0x0FF&str[end+1])<<8;
    }
    if(remainder)
    {
      s[x++] = encode6(buffer>>18);
      s[x++] = encode6(buffer>>12);
      if(remainder==2)
        s[x++] = encode6(buffer>>6);

      if(remainder==1)      
      {
        s[x++] = '=';
        s[x++] = '=';
      }
      else if(remainder==2)
        s[x++] = '=';
    }

    return s;
  }
#else
  std::string Base64Encoder::encode(const std::string &str)
  {
    std::stringstream s;
    int remainder = (int)str.size() % 3;        
    int end = (int)str.size() - remainder;
    long buffer;
    // Every 3 bytes can be encoded in 4x 6-bit characters
    for(int i=0;i<end;i+=3)
    {
      buffer = ((0x0FF&str[i])<<16) | ((0x0FF&str[i+1])<<8) | (0x0FF&str[i+2]);
      s << encode6(buffer>>18)
        << encode6(buffer>>12)
        << encode6(buffer>>6)
        << encode6(buffer);
    }
    if(remainder>=1)
    {
      buffer = (0x0FF&str[end])<<16;
      if(remainder==2)
        buffer |= (0x0FF&str[end+1])<<8;
    }
    if(remainder)
    {
      s << encode6(buffer>>18)
        << encode6(buffer>>12);
      if(remainder==2)
        s << encode6(buffer>>6);

      if(remainder==1)      s << "==";
      else if(remainder==2) s << "=";
    }
    return s.str();
  }
#endif

}
