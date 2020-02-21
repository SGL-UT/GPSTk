#pragma ident "$Id$"

/// @file ScatterPlot.hpp Used to draw a scatter plot. Class declarations.

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

#ifndef VPLOT_SCATTERPLOT_H
#define VPLOT_SCATTERPLOT_H

#include "Color.hpp"
#include "Frame.hpp"
#include "Marker.hpp"

#include "Plot.hpp"
#include "SeriesList.hpp"

using namespace vdraw;

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
      Marker pickNextMarker(void);

      /// Get the idx-th Marker from the generated set
      Marker pickNextMarker(int idx);

      /// Add a series with this label
      inline void addSeries(string label, vector<pair<double,double> >& series)
      {
        addSeries(label,series,pickNextMarker());
      }

      /// Add a series with this label and this Marker
      inline void addSeries(string label, vector<pair<double,double> >& series, Marker m)
      {
        sl.addSeries(label,series,m);
      }

      /// Draw the Plot to this frame
      inline void drawPlot(Frame& frame)
      {
        drawPlot(&frame);
      }

      void drawPlot(Frame* frame)
      {
        Frame innerFrame = getPlotArea(frame);

        double minX, maxX, minY, maxY;
        sl.findMinMax(minX,maxX,minY,maxY);

        // Use default min/max
        sl.drawInFrame(innerFrame,minX,maxX,minY,maxY);

        setXAxis(minX, maxX);
        setYAxis(minY, maxY); 

        drawAxis(frame);
      }

      /** 
       * Draw the Palette key to this frame. dir is the direction from negative
       * to positive for the key.  You probably want to supply a tall, skinny
       * frame for North or South directions and a short and wide frame for East 
       * and West directions. 
       */
      inline void drawKey(Frame& frame, unsigned int columns = 1) 
      {
        drawKey(&frame, columns);
      }

      /** 
       * Draw the Palette key to this frame. dir is the direction from negative
       * to positive for the key.  You probably want to supply a tall, skinny
       * frame for North or South directions and a short and wide frame for East 
       * and West directions. 
       */
      inline void drawKey(Frame* frame, unsigned int columns = 1) 
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
      std::vector<Color> mcvec;

      /// Vector of Marker types
      std::vector<Marker::Mark> mvec;

  };

}

#endif
