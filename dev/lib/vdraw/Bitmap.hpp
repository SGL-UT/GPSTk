#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/Bitmap.hpp#1 $"

/// @file Bitmap.hpp Defines a simple bitmap. Class declarations.

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

#ifndef VDRAW_BITMAP_H
#define VDRAW_BITMAP_H

#include "ColorMap.hpp"
#include "InterpolatedColorMap.hpp"

namespace vdraw
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

   /**
    * This class defines a simple rectangle. The rectangle can
    * have a user defined appearance, specified using a StrokeStyle. 
    */
   class Bitmap {

   public:

      /// Declare the enumeration
      enum CMType { CM, ICM };

      /// X coordinate of first corner
      double x1; 

      /// Y coordinate of first corner
      double y1;

      /// X coordinate of opposite corner
      double x2;

      /// Y coordinate of opposite corner
      double y2;

      /// Row overscaling
      int osr;

      /// Column overscaling
      int osc;

      /// Just draw (way too many) boxes?
      bool boxes;

     /**
      * Constructor. 
      * @param ix1 x coordinate of first corner
      * @param iy1 y coordinate of first corner
      * @param ix2 x coordinate of opposite corner
      * @param iy2 y coordinate of opposite corner
      * @param c Color map to be drawn in
      */
      Bitmap(double ix1, double iy1, double ix2, double iy2, 
          const ColorMap &c);

    /**
      * Constructor. 
      * @param ix1 x coordinate of first corner
      * @param iy1 y coordinate of first corner
      * @param ix2 x coordinate of opposite corner
      * @param iy2 y coordinate of opposite corner
      * @param ic Color map to be drawn in
      */
      Bitmap(double ix1, double iy1, double ix2, double iy2, 
          const InterpolatedColorMap &ic);

      /// Copy constructor
      Bitmap(const Bitmap& o);

      /// Destructor
      ~Bitmap() {}

      /// Get the ColorMap and return true if that is what this was
      /// initialized with
      bool getCM(ColorMap *c) const 
      {
        if(which!=CM) return false;
        *c = cm;
        return true;
      }

      /// Get the InterpolatedColorMap and return true if that is what this was
      /// initialized with
      bool getICM(InterpolatedColorMap *c) const 
      {
        if(which!=ICM) return false;
        *c = icm;
        return true;
      }

      /// Oversample such that each color is represented in scale^2 pixels.
      inline void oversample(int scale) { oversample(scale,scale); }

      /**
       * Scaling such that 1 colormap entry = xscale*yscale final image entries
       * in the output image.
       * @param rscale 1 ColorMap row mapped to rscale image rows
       * @param cscale 1 ColorMap column mapped to cscale image columns
       */
      inline  void oversample(int rscale, int cscale)
      {
        osr = rscale;
        osc = cscale;
        if(osr<1) osr=1;
        if(osc<1) osr=1;        
      }

      /// Whether or not to just draw boxes
      inline void useBoxes(bool useboxes) { boxes = useboxes; }

   protected:

   private:

      /// Private function call to make the first coordinate (x1,y1) the bottom
      /// left corner and make the opposite corner(x2,y2) reflect the change.
      void fixPoints();

      // ColorMap information
      CMType which;
      ColorMap cm;
      InterpolatedColorMap icm;
   }; // class Bitmap

   //@}

} // namespace vdraw

#endif //VDRAW_BITMAP_H


