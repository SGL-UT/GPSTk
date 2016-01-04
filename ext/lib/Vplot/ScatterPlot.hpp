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

/// @file ScatterPlot.hpp Used to draw a scatter plot. Class declarations.

#ifndef VPLOT_SCATTERPLOT_H
#define VPLOT_SCATTERPLOT_H

#include "Color.hpp"
#include "Frame.hpp"
#include "Marker.hpp"

#include "Plot.hpp"
#include "SeriesList.hpp"


namespace vplot
{
      /**
       * This class draws a basic scatter plot to a frame.  
       */
   class ScatterPlot : public Plot
   {
   public:
         /**
          * Constructor.
          */
      ScatterPlot(unsigned int iidx=0) : Plot()
      {
         init(iidx);
      }

         /// Destructor
      ~ScatterPlot()
      {
      }

         /// Initialize the StrokeStyle
      void init(unsigned int imidx);

         /// Get the next Marker from the generated set      
      vdraw::Marker pickNextMarker(void);

         /// Get the idx-th Marker from the generated set
      vdraw::Marker pickNextMarker(int idx);

         /// Add a series with this label
      inline void addSeries(std::string label,
                            std::vector<std::pair<double,double> >& series)
      {
         addSeries(label,series,pickNextMarker());
      }

         /// Add a series with this label and this Marker
      inline void addSeries(std::string label,
                            std::vector<std::pair<double,double> >& series,
                            vdraw::Marker m)
      {
         sl.addSeries(label,series,m);
      }

         /// Draw the Plot to this frame
      inline void drawPlot(vdraw::Frame& frame)
      {
         drawPlot(&frame);
      }

      void drawPlot(vdraw::Frame* frame)
      {
         vdraw::Frame innerFrame = getPlotArea(frame);

         double minX, maxX, minY, maxY;
         sl.findMinMax(minX,maxX,minY,maxY);

            // Use default min/max
         sl.drawInFrame(innerFrame,minX,maxX,minY,maxY);

         setXAxis(minX, maxX);
         setYAxis(minY, maxY); 

         drawAxis(frame);
      }

         /** 
          * Draw the Palette key to this frame. dir is the direction
          * from negative to positive for the key.  You probably want
          * to supply a tall, skinny frame for North or South
          * directions and a short and wide frame for East and West
          * directions.
          */
      inline void drawKey(vdraw::Frame& frame, unsigned int columns = 1) 
      {
         drawKey(&frame, columns);
      }

         /** 
          * Draw the Palette key to this frame. dir is the direction
          * from negative to positive for the key.  You probably want
          * to supply a tall, skinny frame for North or South
          * directions and a short and wide frame for East and West
          * directions.
          */
      inline void drawKey(vdraw::Frame* frame, unsigned int columns = 1) 
      {
         drawLegend(frame,&sl, columns);
      }


   protected:


   private:

         /// List of series
      SeriesList sl;

         /// Marker index and initializer
      unsigned int midx;
      unsigned int imidx;

         /// Vector of colors
      std::vector<vdraw::Color> mcvec;

         /// Vector of Marker types
      std::vector<vdraw::Marker::Mark> mvec;

   };

}

#endif
