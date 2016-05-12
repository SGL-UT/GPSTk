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

/// @file InterpolatedColorMap.hpp Defines an interpolated color map. Class declarations.

#ifndef VDRAW_INTERPOLATEDCOLORMAP_H
#define VDRAW_INTERPOLATEDCOLORMAP_H

#include "Color.hpp"
#include "Palette.hpp"

namespace vdraw
{
      /// @ingroup BasicVectorGraphics
      //@{

      /**
       * This class represents a 2d pixel map of colors.  Each color
       * is represented by an index into the given palette.
       */
   class InterpolatedColorMap
   {
   public:
         /**
          * Default constructor.
          */
      InterpolatedColorMap()
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
      InterpolatedColorMap(int icols, int irows, const Palette &pp,
                           double base=0.0);

         /// Copy constructor.
      InterpolatedColorMap(const InterpolatedColorMap &o);

         /// Destructor
      ~InterpolatedColorMap() { reset(); }

         /**
          * Copy operator.  
          *
          * This is in replacement for operators like:
          * - InterpolatedColorMap& operator=(const InterpolatedColorMap& o);
          * 
          * As this is what happens:
          * \code
          * InterpolatedColorMap ic(...);  
          * ...; //init ic
          * InterpolatedColorMap c;
          * c = ic; // c.operator=(ic);
          * InterpolatedColorMap c2 = ic // c2(ic);
          * \endcode
          */
      InterpolatedColorMap& operator=(InterpolatedColorMap o);

         /// Set the color at a row and column
      inline void setColor(int row, int col, double f) { c[row][col]=f; }

         /// Get the color at a row and column
      inline Color get(int row, int col) const { return p.getColor(getIndex(row,col)); }

         /// Get the color index at a row and column
      inline double getIndex(int row, int col) const  { return c[row][col]; }

         /// Get the cols of the map
      inline int getCols() const { return cols; }

         /// Get the rows of the map
      inline int getRows() const { return rows; }

         /// Get Palette
      inline const Palette getPalette() const { return p; }

   protected:
         /// Initialization helper
      void init(int icols, int irows);

         /// Reset helper
      void reset();

         /// Width of the map
      int cols;

         /// Height of the map
      int rows;

         /// The color palette
      Palette p;

         /// Index array
      double **c;
   }; // class InterpolatedColorMap

      //@}

} // namespace vdraw

#endif // VDRAW_INTERPOLATEDCOLORMAP_H
