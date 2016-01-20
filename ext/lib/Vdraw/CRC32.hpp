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

/// @file CRC32.hpp Used to calculate a CRC-32 checksum. Class declarations.

#ifndef VDRAW_CRC32_H
#define VDRAW_CRC32_H

#include <string>

// Based on
// http://burtleburtle.net/bob/c/crc.c
// and
// http://www.csbruce.com/~csbruce/software/crc32.c

namespace vdraw
{
  /// @ingroup BasicVectorGraphics  
  //@{

  /**
   * This class is used to generate a CRC-32 checksum for a sequence of bytes.
   */
  class CRC32
  {
    protected:
      /// Table used to compute the CRC32 value
      static const unsigned int crc_table[256];

      /// The current CRC32 value
      unsigned int crc32;

    public:
      /// Default constructor
      CRC32()
      {
        reset();
      }

      /// Resets the CRC32 to initial value
      inline void reset()
      {
        crc32 = 0xffffffff;
      }

      /// Get the CRC32 checksum
      inline unsigned int getValue() const
      {
        return crc32 ^ 0xffffffff;
      }

      /**
       * Updates the CRC32 with specified string
       * @param  str The string to update the CRC32 with
       */
      void update(const std::string &str)
      {
        update(str.c_str(),str.length());
      }

      /**
       * Updates the CRC32 checksum with specified array of bytes
       * @param buf The byte array to update the CRC32 with
       * @param len The length of the byte array
       */
      void update(const char* buf, unsigned int len);

  }; // class CRC32

  //@}

} // namespace vdraw

#endif //VDRAW_CRC32_H
