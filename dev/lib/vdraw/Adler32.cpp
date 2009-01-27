#pragma ident "$Id:$"

/// @file Adler32.cpp Used to calculate an Adler-32 checksum. Class definitions.

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

//http://en.wikipedia.org/wiki/Adler-32

#include "Adler32.hpp"

namespace vdraw
{
  void Adler32::update(const char* buf, unsigned int len)
  {
    if (buf == 0 || len == 0)
      return;

    // Go in order...
    for(unsigned int i=0;i<len;i++)
    {
      a = (a+(((int)buf[i])&0x000000FF)) % mod;
      b = (b+a) % mod;
    }
  }

} // namespace vdraw


