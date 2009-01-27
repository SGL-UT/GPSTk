#pragma ident "$Id$"

/// @file PNG.hpp Create PNG images.  Class declarations.

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


#ifndef VDRAW_PNG_H
#define VDRAW_PNG_H

#include<string>
#include<sstream>
#include<cmath>
#include<memory>

#include "Bitmap.hpp"
#include "Color.hpp"
#include "ColorMap.hpp"
#include "InterpolatedColorMap.hpp"
#include "Palette.hpp"
#include "CRC32.hpp"
#include "Adler32.hpp"

namespace vdraw
{
  /** \addtogroup BasicVectorGraphics */ 
  //@{

  /**
   * Used to create PNG Images.
   */
  class PNG
  {
    public:
      /// Typedef of string pointer using auto_ptr
      typedef std::auto_ptr<std::string> string_ptr;

      /**
       * Get a string representing the contents of a PNG file using a Bitmap.
       * This is a helper function to call the others.
       */
      static std::string png(const Bitmap &b);

      /**
       * Get a string representing the contents of a PNG file using an indexed
       * color map.
       */
      static string_ptr png(const ColorMap &c, int osr=1, int osc=1);

      /**
       * Get a string representing the contents of a PNG file using an indexed
       * color map.
       */
      static string_ptr png(const InterpolatedColorMap &c, int osr=1, int osc=1);

      /// Get the byte cost of an indexed PNG
      /// Returns -1 if the bitmap doesn't have an Indexed/InterpolatedColorMap
      static int cost_indexed(const Bitmap& b);

      /// Get the byte cost of an indexed PNG with numcol colors
      static int cost_indexed(int rows, int cols, int numcol);

      /// Get the byte cost of a non-indexed PNG
      static int cost_constant(const Bitmap& b);

      /// Get the byte cost of a non-indexed PNG
      static int cost_constant(int rows, int cols);

    protected:
      /// PNG file format header bytes 
      static const std::string header;

      /// Get the cost for IDAT chunks for a stream of this length
      static int cost_idat(int stream);

      /// IHDR Chunk for full color maps
      static string_ptr ihdr_full(int width, int height);

      /// IHDR Chunk for indexed color maps
      static string_ptr ihdr_indexed(int width, int height);

      /// sRGB Chunk for telling the PNG renderer to keep the colors pretty
      static string_ptr srgb();

      /// pHYs Chunk for telling the PNG renderer pixel width/height
      static string_ptr phys();

      /// PLTE Chunk for listing the indexed colors
      static string_ptr plte(const InterpolatedColorMap &c);

      /*
       * ZLIB flag information
       * First two bytes:
       * +-----+-----+
       * | CMP | FLG |
       * +-----+-----+
       * CMP:
       *  - Bits 0-3 = 0x08 (deflate compression)
       *  - Bits 4-7 = For CM = 8, CINFO is the base-2 logarithm 
       *    of the LZ77 window size, minus eight
       *
       */

      /// IDAT Chunk for an indexed color map
      static string_ptr idat(const InterpolatedColorMap &c, int osr, int osc);

      /// IDAT Chunk for a full color map      
      static string_ptr idat(const ColorMap &c, int osr, int osc);

      /// Split up IDAT Chunk to multiple chunks if too large
      static string_ptr split(const std::string &str);

      /// Add the IDAT prefix bytes for the zlib stream
      static string_ptr prefix(const std::string &str);

      /// Add the segment bits for the huffman format (non compressed)
      static string_ptr huff(const std::string &str);

      /// Get the color data for a full color map
      static string_ptr data(const ColorMap &c, int osr, int osc);

      /// Get the color data for an indexed color map      
      static string_ptr data(const InterpolatedColorMap &c, int osr, int osc);

      /// Get the alder32 checksum for a string
      static unsigned int alder(const string_ptr &str);

      /// IEND Chunk required at end of PNG
      static string_ptr iend();

      /// Add chunk bits (length, checksum) before and after chunk bytes
      static string_ptr chunk(const std::string &title, const std::string &text);

      /// Integer to string of 4 bytes
      static string_ptr itos(int i);

      /// Color to string of 3 bytes
      static string_ptr ctos(const Color &color);

      /// Byte to single character
      static char btoc(int b);

  }; // class PNG

  //@}

} // namespace vdraw

#endif //VDRAW_PNG_H


