#pragma ident "$Id$"

/// @file IndexedColorMap.hpp Defines an indexed color map. Class declarations.

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

#ifndef VDRAW_INDEXEDCOLORMAP_H
#define VDRAW_INDEXEDCOLORMAP_H

#include "Color.hpp"
#include "Palette.hpp"

namespace vdraw
{
  /** \addtogroup BasicVectorGraphics */ 
  //@{

  /**
   * This class represents a 2d pixel map of colors. Each color is represented 
   * by an index into the given palette.
   */
  class IndexedColorMap
  {
    public:
      /**
       * Default constructor.
       */
      IndexedColorMap()
      {
        cols=rows=0;
        c=0;
      }

      /**
       * Constructor.
       * @param icols Width of the map
       * @param irows Height of the map
       * @param pp Palette to use 
       * @param base Base color for the map to be initialized to
       */
      IndexedColorMap(int icols, int irows, const Palette &pp, int base=0);

      /// Destructor
      ~IndexedColorMap() { reset(); }

      /// Set the color at a row and column
      inline void setColor(int row, int col, int f) { c[row][col]=f; }

      /// Get the color at a row and column
      inline Color get(int row, int col) const { return p.getColor(getIndex(row,col)); }

      /// Get the color index at a row and column
      inline int getIndex(int row, int col) const { return c[row][col]; }

      /// Get the cols of the map
      inline int getCols() const { return cols; }

      /// Get the rows of the map
      inline int getRows() const { return rows; }

      /// Get Palette
      inline const Palette getPalette() const { return p; }

      /// Reset the IndexedColorMap
      void reset();

    protected:
      /// Width of the map
      int cols;

      /// Height of the map
      int rows;

      /// The color palette
      Palette p;

      /// Index array
      int **c;
  }; // class IndexedColorMap

  // @}

} // namespace vdraw

#endif // VDRAW_INDEXEDCOLORMAP_H

