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

/// @file Plot.hpp Basic plot information. Class declarations.

#ifndef VPLOT_PLOT_H
#define VPLOT_PLOT_H

#include <string>

#include "TextStyle.hpp"
#include "StrokeStyle.hpp"
#include "BorderLayout.hpp"
#include "Frame.hpp"

#include "Axis.hpp"
#include "AxisStyle.hpp"
#include "SeriesList.hpp"


namespace vplot
{
      /**
       * This class contains several things that are similar between a
       * few types of plots.  This includes some axis and label
       * information and styles.
       */
   class Plot
   {
   public:
         /// Constructor
      Plot();

         /// Destructor
      ~Plot() 
      {
      }

         /*
          * Public methods
          */

         /// Set the minimum and maximum for x axis (doesn't effect columns)
      inline void setXAxis(double min, double max, double gap=0) 
      { minx=min; width=max-min; xgap=gap; fixedXaxis = true;}
         /// Set the minimum and maximum for y axis (doesn't effect rows)
      inline void setYAxis(double min, double max, double gap=0) 
      { miny=min; height=max-min; ygap=gap; fixedYaxis = true; }
         /// Set the label for the x axis
      inline void setXLabel(const char* str) { xlabel = std::string(str); }
         /// Set the label for the x axis
      inline void setXLabel(const std::string& str) { xlabel = std::string(str); }
         /// Set the label for the y axis
      inline void setYLabel(const char* str) { ylabel = std::string(str); }
         /// Set the label for the y axis
      inline void setYLabel(const std::string& str) { ylabel = std::string(str); }

         /// Get the Frame around which the axes will be drawn.  
      vdraw::Frame getPlotArea(vdraw::Frame& frame)
      {
         return getPlotArea(&frame);
      }

         /// Get the Frame around which the axes will be drawn.        
      vdraw::Frame getPlotArea(vdraw::Frame* frame);

         /// Draw the axis in this Frame
      void drawAxis(vdraw::Frame& frame)
      {
         drawAxis(&frame);
      }

         /// Draw the axis in this Frame
      void drawAxis(vdraw::Frame* frame);

         /* 
          * Settings / options
          */

         /// TextStyle for the labels
      vdraw::TextStyle label_style;

         /// Style for the north axis
      AxisStyle axis_top;

         /// Style for the south axis
      AxisStyle axis_bottom;

         /// Style for the left axis
      AxisStyle axis_left;

         /// Style for the right axis
      AxisStyle axis_right;


   protected:
         /// X axis label
      std::string xlabel;

         /// User has set the Y axis
      bool fixedYaxis;
 
         /// User has set the X axis
      bool fixedXaxis;

         /// Y axis label
      std::string ylabel;

         /// Minimum x of the plot
      double minx;

         /// Width of the plot
      double width;

         /// Gap from minimum x
      double xgap;

         /// Minimum y of the plot
      double miny;

         /// Height of the plot
      double height;

         /// Gap from minimum y
      double ygap;

         /**
          * Draw the legend in this Frame to some degree.  Since it is
          * difficult to determine the width of a specific string, for
          * now this merely makes a single column of legend.  This
          * should be good for most generic plots.
          */
      void drawLegend(vdraw::Frame* frame, SeriesList* sl,
                      unsigned int columns = 1)
      {
         sl->drawLegend(*frame, 10, columns);
      }
   };

}

#endif
