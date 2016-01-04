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

/// @file SurfacePlot.hpp Used to draw a surface plot. Class declarations.

#ifndef VPLOT_SURFACEPLOT_H
#define VPLOT_SURFACEPLOT_H

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <cmath>
#include <limits.h>
#include <algorithm>

#include "Bitmap.hpp"
#include "ColorMap.hpp"
#include "InterpolatedColorMap.hpp"
#include "Frame.hpp"
#include "HLayout.hpp"
#include "StrokeStyle.hpp"
#include "Path.hpp"
#include "BorderLayout.hpp"
#include "Palette.hpp"

#include "Axis.hpp"
#include "AxisStyle.hpp"
#include "Plot.hpp"


namespace vplot
{
      /**
       * This class helps to create a simple Surface Plot.
       */
   class SurfacePlot : public Plot
   {
   public:

         /**
          * Constructor.
          * @param iwidth Width of color map
          * @param iheight Height of the color map
          * @param p Palette for the elements in this surface map
          */
      SurfacePlot(int iwidth, int iheight, const vdraw::Palette& p);

         /// Destructor
      ~SurfacePlot();

         /*
          * Public methods
          */
         /// Set the label for the color axis
      inline void setColorLabel(const char* str)
      { colorlabel = std::string(str); }

         /// Draw the Plot to this frame, with the key on the dir side
      void draw(vdraw::Frame *frame, int dir);

         /// Draw the Plot to this frame
      void drawPlot(vdraw::Frame* frame);

         /** 
          * Draw the Palette key to this frame. dir is the direction
          * from negative to positive for the key.  You probably want
          * to supply a tall, skinny frame for North or South
          * directions and a short and wide frame for East and West
          * directions.
          */
      void drawKey(vdraw::Frame *frame); //, int dir);

         /// Set the color at row,col to the color at d on the palette.
      inline void set(int row, int col, double d)
      {
         icm.setColor(row,col,d);
      }

         /// Get the index for a row and column 
      inline double get(int row, int col)
      {
         return icm.getIndex(row,col);
      }

         /// Set up oversampling 
      inline void oversample(int scale)
      { oversample(scale,scale); }

         /// Set up oversampling
      inline void oversample(int rscale, int cscale)
      { osr = rscale; osc = cscale; }

         /// Use boxes when rendering the surface 
      inline void useBoxes(bool b=true)
      { boxes = b; }

         /* 
          * Settings / options
          */
      
         /// Style for the right axis
      AxisStyle axis_color;

   protected:

   private:
         /// Color axis label
      std::string colorlabel;

         /// Overscale rows 
      int osr;

         /// Overscale columns
      int osc;

         /// Use boxes instead of a compressed image
      bool boxes;

         /// ColorMap
      vdraw::InterpolatedColorMap icm;

   };

}

#endif  //VPLOT_SURFACEPLOT_H
