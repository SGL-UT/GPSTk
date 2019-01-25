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

/// @file Palette.hpp Defines a color palette. Class declarations.

#ifndef VDRAW_PALETTE_H
#define VDRAW_PALETTE_H

#include <algorithm>

#include "Color.hpp"

namespace vdraw
{
      /// @ingroup BasicVectorGraphics
      //@{

      /**
       * This class defines a simple interpolated palette of colors.
       * Each entry in the pallete is a color at a certain position.
       * Indexes into this palette can be anywhere within its range
       * and the corresponding color will be interpolated.
       */

   class Palette
   {
   public:
         /**
          * Constructor.
          * @param base Color to put at the extremes of the palette
          * @param imin Minimum value of the palette's indicies
          * @param imax Maximum value of the palette's indicies
          */
      Palette(const Color &base=Color::BLACK, double imin=0.0, double imax=1.0);

         /// Copy Constructor.
      Palette(const Palette &p);

         /// Copy Operator.
      Palette& operator=(Palette p);

         /// Destructor
      ~Palette()
      {
      }

         /// Set the range 
         /// Note: Does not change indicies already in the palette
      void setRange(double imin, double imax)
      {
         if(imax<imin) 
         { 
            min = imax; 
            width=imin-imax; 
         }
         else
         {
            min = imin;
            width = imax-imin;
         }
      }

         /// Set the color a val to c
      void setColor(double val, const Color &c);

         /// Get the color at val
      Color getColor(double val) const;

         /// Get the minimum
      inline double getMin() const { return min; }

         /// Get the maximum
      inline double getMax() const { return min+width; }

         /// Get number of entries
      inline int getNumColors() const { return (int)palette.size(); }

   protected:
         /// Minimum
      double min;
      
         /// Width of the palette (such that: max = min+width)
      double width;

         /// The (index,color) pairs of the palette
      std::list<std::pair<double,Color> > palette;

         /// Clamp the value between the minimum and maximum
      inline void clamp(double &val) const
      {
         if(val<min)         val=getMin();
         if(val>(min+width)) val=getMax();
      }
   }; // class Palette

      //@}
  
} // namespace vdraw

#endif // VDRAW_PALETTE_H
