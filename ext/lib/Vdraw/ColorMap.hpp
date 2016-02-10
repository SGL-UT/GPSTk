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

/// @file ColorMap.hpp Defines a color map. Class declarations.

#ifndef VDRAW_COLORMAP_H
#define VDRAW_COLORMAP_H

#include <algorithm>

#include "Color.hpp"
#include "Palette.hpp"
//#include "IndexedColorMap.hpp"
#include "InterpolatedColorMap.hpp"

namespace vdraw
{
  /// @ingroup BasicVectorGraphics  
  //@{

  /**
   * This class represents a 2d pixel map of colors.  
   */
  class ColorMap
  {
    public:
      /// Default constructor.
      ColorMap()
      {
        cols=rows=0;
        c=0;
      }

      /// Copy constructor.
      ColorMap(const ColorMap &o);

      /// Convert constructor.
      ColorMap(const InterpolatedColorMap &o);

      /// Convert constructor.
      //ColorMap(const IndexedColorMap &o);

      /// Convert constructor.
      ColorMap(const Palette &p, double dir=90, int num=256)
      {
        // TODO Figure out direction, initialize accordingly
        // For now we'll assume NORTH

        if(num<=0) num = 256;

        init(1,num);
        
        for(int row=0;row<rows;row++)
          for(int col=0;col<cols;col++)
            c[row][col] = p.getColor((1.0*row)/rows);
      }

      /**
       * Constructor.
       * @param icols Width of the map
       * @param irows Height of the map
       * @param base Base color for the map to be initialized to
       */
      ColorMap(int icols, int irows, const Color &base=Color::BLACK);

      /// Destructor
      ~ColorMap() { reset(); }

      /**
       * Copy operator.  
       *
       * This is in replacement for operators like:
       * - ColorMap& operator=(const ColorMap& o);
       * - ColorMap& operator=(const InterpolatedColorMap& ic);
       * 
       * As this is what happens:
       * \code
       * InterpolatedColorMap ic(...);  
       * ...; //init ic
       * ColorMap c;
       * c = ic; // c.operator=(ColorMap(ic));
       * ColorMap c2 = ic // c2(ic);
       * \endcode
       */
      ColorMap& operator=(ColorMap o);      

      /// Set the color at a row and column
      inline void setColor(int row, int col, const Color& color) { c[row][col]=color; }

      /// Get the color at a row and column
      inline Color get(int row, int col) const { return c[row][col]; }

      /// Get the cols of the map
      inline int getCols() const { return cols; }

      /// Get the rows of the map
      inline int getRows() const { return rows; }

    protected:
      /// Initialization helper
      void init(int icols, int irows);

      /// Reset helper
      void reset();

      /// Width of the map
      int cols;

      /// Height of the map
      int rows;

      /// Color array
      Color **c;
  }; // class ColorMap

  //@}

} // namespace vdraw

#endif // VDRAW_COLORMAP_H
