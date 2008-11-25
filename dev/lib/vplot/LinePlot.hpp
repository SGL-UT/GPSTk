#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/plot/LinePlot.hpp#7 $"

/// @file LinePlot.hpp Used to draw a line plot. Class declarations.

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

#ifndef VPLOT_LINEPLOT_H
#define VPLOT_LINEPLOT_H

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <cmath>
#include <limits.h>
#include <algorithm>

#include "Frame.hpp"
#include "StrokeStyle.hpp"
#include "Path.hpp"
#include "BorderLayout.hpp"
#include "HLayout.hpp"

#include "Plot.hpp"
#include "SeriesList.hpp"

using namespace std;
using namespace vdraw;

namespace vplot
{
  /**
   * This class draws a basic line plot to a frame.  
   *
   * \FIXME This is retrofitted from an older implementation and needs to be made
   * up to date with the style of these plotting classes (with more
   * configurability)
   */
  class LinePlot : public Plot
  {
    public:
      /**
       * Constructor.
       */
      LinePlot(unsigned int iidx=0) : Plot()
      {
        init(iidx);
      }

      /// Destructor
      ~LinePlot()
      {
      }

      /// Initialize the StrokeStyle
      void init(unsigned int issidx);

      /// Get the next StrokeStyle from the generated set      
      StrokeStyle pickNextSS(void);

      /// Get the idx-th StrokeStyle from the generated set
      StrokeStyle pickNextSS(int idx);

      /// Add a series with this label
      void addSeries(string label, vector<pair<double,double> >& series)
      {
        addSeries(label,series,pickNextSS());
      }

      /// Add a series with this label and this StrokeStyle      
      void addSeries(string label, vector<pair<double,double> >& series, StrokeStyle ss)
      {
        sl.addSeries(label,series,ss);
      }

      /// Draw the Plot to this frame, with the key on the dir side
      void draw(Frame& frame, int dir)
      {
        draw(&frame,dir);
      }

      /// Draw the Plot to this frame, with the key on the dir side
      void draw(Frame *frame, int dir);

      /// Draw the Plot to this frame
      void drawPlot(Frame& frame)
      {
        drawPlot(&frame);
      }

      /// Draw the Plot to this frame
      void drawPlot(Frame* frame);

      /** 
       * Draw the Palette key to this frame. dir is the direction from negative
       * to positive for the key.  You probably want to supply a tall, skinny
       * frame for North or South directions and a short and wide frame for East 
       * and West directions. 
       */
      void drawKey(Frame& frame) //, int dir);
      {
        drawKey(&frame);
      }

      /** 
       * Draw the Palette key to this frame. dir is the direction from negative
       * to positive for the key.  You probably want to supply a tall, skinny
       * frame for North or South directions and a short and wide frame for East 
       * and West directions. 
       */
      void drawKey(Frame* frame) //, int dir);
      {
        // TODO 
      }

    protected:


    private:

      /// List of series
      SeriesList sl;

      /// StrokeStyle index and initializer
      unsigned int ssidx;
      unsigned int issidx;

      /// Vector of colors
      std::vector<Color> cvec;

      /// Vector of dashed line types
      std::vector<StrokeStyle::dashLengthList> dvec;

  };

}

#endif
