#pragma ident "$Id$"

/// @file Axis.hpp Used to draw an axis. Class declarations.

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

#ifndef VPLOT_AXIS_H
#define VPLOT_AXIS_H

#include <cmath>
#include "AxisStyle.hpp"
#include "GraphicsConstants.hpp"
#include "Frame.hpp"

/**
 * Drawing basic plots using the classes in the vdraw library.
 */
namespace vplot
{
  /*
   * I probably need a class for the following things:
   *   - Direction for axis
   *   - Bounds!
   *     - Bound generation from min/max (with buffer area? round?)
   * - If this is a seperate class it needs a set of COPY commands
   *   that copy to a different orientation.  Or a mirror setting that
   *   does so automagically.
   */

  /**
   * This class is used to draw a simple axis.  It can be configured mostly 
   * by using an AxisStyle but specific things as to this particular axis are
   * modified in this class.
   */
  class Axis
  {
    public:
      static const double RIGHT; // = 0;
      static const double UP   ; // = RIGHT+vdraw::HALF_PI;
      static const double LEFT ; // = UP+vdraw::HALF_PI;
      static const double DOWN ; // = LEFT+vdraw::HALF_PI;
      static const double N    ; // = UP;
      static const double E    ; // = RIGHT;
      static const double S    ; // = DOWN;
      static const double W    ; // = LEFT;
      static const double NORTH; // = N;
      static const double EAST ; // = E;
      static const double SOUTH; // = S;
      static const double WEST ; // = W;

      /*
       * Constructor/Destructor
       */

      /**
       * Constructor.
       * @param ix Start x position
       * @param iy Start y position
       * @param ex End x position
       * @param ey End y position
       * @param length Length of the axis (in frame space)
       * @param imin Minimum of the axis
       * @param imax Maximum of the axis
       * @param style Style of the axis
       */
      Axis(double ix, double iy, double ex, double ey, double length, double imin, double imax, const AxisStyle &style=AxisStyle())
      {
        //Calculate direction
        /*
         * tan(dir) = opposite / adjacent;
         * arctan(opposite/adjacent) = dir;
         * opposite = (ey-iy);
         * adjacent = (ex-ix);
         */
        double direction;
        if(ex==ix) 
        {
          if(ey < iy) direction=vdraw::PI;
          else        direction=0;
        }
        else if(ey==iy)
        {
          if(ex < ix) direction=vdraw::PI+vdraw::HALF_PI;
          else        direction=vdraw::HALF_PI;
        }
        else
        {
          direction=atan((ey-iy)/(ex-ix));
          if(direction < 0) direction=vdraw::TWO_PI+direction;
        }
        init(ix,iy,direction,length,imin,imax,style);
      }

      /**
       * Constructor.
       * @param ix Start x position
       * @param iy Start y position
       * @param direction Direction the axis moves toward
       * @param length Length of the axis (in frame space)
       * @param imin Minimum of the axis
       * @param imax Maximum of the axis
       * @param style Style of the axis
       */
      Axis(double ix, double iy, double direction, double length, double imin, double imax, const AxisStyle &style=AxisStyle())
      {
        init(ix,iy,direction,length,imin,imax,style);
      }

      /**
       *
       */
      ~Axis() 
      {
      }

      /*
       * Accessors/Modifiers
       */
      /// Set the x and y position
      inline void setPosition(double ix, double iy) { x=ix; y=iy; }

      /// Set the length
      inline void setLength(double length) { line_length=length; }

      /// Set the range
      inline void setRange(double imin, double imax) { min=imin; max=imax; axesMin=min; axesMax=max; }

      /// Set the gap (distance in plot space between ticks)
      inline void setGap(double igap) { gap=igap; }

      /// Set the angle for the axis
      void setAngle(double angle);

      /*
       * Methods
       */

      /**
       * Draw this zxis to this frame
       * @param ff Pointer to frame to draw to
       */
      void drawToFrame(const vdraw::Frame &ff);

      /**
       * Finds out the x, y point given a value.
       * @param[out] x 
       * @param[out] y
       * @param[in] value
       * @return 
       */
      bool pointFromValue(double &x, double &y, double value);

      /**
       * Finds out the x, y point for a label above the axis given a value.
       * @param[out] x 
       * @param[out] y
       * @param[out] rotation
       * @param[in] value
       * @return 
       */
      inline bool labelPointAbove(double &x, double &y, double &rotation, double value)
      {
        return labelPoint(x,y,rotation,value,AxisStyle::ABOVE);
      }

      /**
       * Finds out the x, y point for a label below the axis given a value.
       * @param[out] x 
       * @param[out] y
       * @param[out] rotation
       * @param[in] value
       * @return 
       */
      inline bool labelPointBelow(double &x, double &y, double &rotation, double value)
      {
        return labelPoint(x,y,rotation,value,AxisStyle::BELOW);
      }

      /// Axis style
      AxisStyle axis_style;

    protected:
      /*
       * Funtions
       */

      /**
       * Initializer (the real constructor).
       * @param ix Start x position
       * @param iy Start y position
       * @param direction Direction the axis moves toward
       * @param length Length of the axis (in frame space)
       * @param imin Minimum of the axis
       * @param imax Maximum of the axis
       * @param style Style of the axis
       */
      void init(double ix, double iy, double direction, double length, double imin, double imax, const AxisStyle &style);

      /*
       * Old idea for a helper to draw ticks.  I like my ascii art too much to
       * delete it for now :)
       *
       * For example:
       *        sr=.5   sv=0     |--1--|=fd
       * |-------50%-------|     |--5--|=rd
       * |-----|-----|-----|-----|-----|-----| <-- Actual Line
       * -3    -2    -1    0     1     2     3 <-- Actual labels
       * Would call as follows:
       * axisHelper(f,0.5,0,5,1)
       *
       * sr Start ratio (% of length to begin point)
       * sv Start value (in plot space)
       * rd Real distance (in page space)
       * fd Fake distance (in plot space)
       */

      /**
       * Finds out the point for the ratio along the axis line.
       * @param[out] x 
       * @param[out] y
       * @param[in] ratio
       */
      inline void fromRatio(double &x, double &y, double ratio)
      {
        x = this->x+cosdir*line_length*ratio;
        y = this->y+sindir*line_length*ratio;
      }

      /**
       * Helper to start the recursive method.
       * @param ratio Ratio on axis line where tick is.
       * @param dist Distance between major ticks
       */
      inline void axisTick(double ratio, double dist)
      {
        axisTickRecursive(
            true,
            axis_style.tick_recursion_depth,
            ratio,
            dist,
            axis_style.major_tick_length);
      }

      /**
       * Recursively draw the axis ticks.
       * @param draw Whether or not to draw this tick
       * @param depth Depth that this tick is (used in recursion)
       * @param ratio Ratio on axis line where tick is. 0 <= ratio <= 1
       * @param dist Ratio distance between this tick and the next one with same depth
       * @param length Length of the tick
       */
      void axisTickRecursive(bool draw, int depth, double ratio, double dist, double length);

      /**
       * Draw a tick at this ratio with this length
       * @param ratio Where the tick is. 0 <= ratio <= 1
       * @param length Length of the tick
       */
      void drawTick(double ratio, double length);

      /**
       * Get the endpoints of a tick at ratio
       * @param[out] x1 Upper x output
       * @param[out] y1 Upper y output
       * @param[out] x2 Lower x output
       * @param[out] y2 Lower y output
       * @param[in] distratio Distance up axis line, ratio to length
       * @param[in] length Length of tick mark
       */
      void fromTic(double &x1, double &y1, double &x2, double &y2, 
          double distratio, double length);

      /**
       * The major axis ticks will start at the the startpos ratio with value
       * startval and at every increment of distpos there will be the previous
       * value + distval.  
       * The *pos are RATIOS of the full width/height.  
       * The *val are values of the data.  
       * Note that positions that are less than 0 or greater than 1 are not
       * themselves drawn on the axis, but can be important as the recursively
       * smaller sub-ticks might be within the 0-1 range.
       * @param[out] startpos ratio position (can be <0) of start position
       * @param[out] startval value at the start position
       * @param[out] distpos ratio distance between positions 
       * @param[out] distval value distance between each major tick
       */
      void guessTickInfo(double &startpos, double &startval, double &distpos, double &distval);

      /**
       * Put a label at this position.
       * @param value The value at which the label will go
       * @param direction Direction where the label is AxisStyle::(ABOVE|BELOW)
       * @param style TextStyle of the label
       */
      bool label(double value, int direction, const vdraw::TextStyle &style);

      /**
       * Put a label at this position.
       * @param str The string for the label
       * @param value The value at which the label will go
       * @param direction Direction where the label is AxisStyle::(ABOVE|BELOW)
       * @param style TextStyle of the label
       */
      bool label(const char *str, double value, int direction, const vdraw::TextStyle &style);

      /**
       * Put a label at this position.
       * @param t Text object for this label (will be modified)
       * @param value The value at which the label will go
       * @param direction Direction where the label is AxisStyle::(ABOVE|BELOW)
       * @param style TextStyle of the label
       */
      bool label(vdraw::Text &t, double value, int direction, const vdraw::TextStyle &style);

      /**
       * Get the x,y position for a label and its rotation
       * Note that rotation needs to be corrected to be right-side up
       */
      bool labelPoint(double &x, double &y, double &rotation, double value, int direction);

      /*
       * This stuff is specific to the axis in its own right.
       */
      /// Temporary frame pointer (so we don't have to pass it to every
      /// function)
      vdraw::Frame f;

      /// X coordinate of this axis
      double x;

      /// Y coordinate of this axis
      double y;

      /// Length of this axis
      double line_length;

      /// Direction is an angle in radians (for use with sin, cos)
      double line_direction;
      /// Sin(line_direction)
      double sindir;
      /// Cos(line_direction)
      double cosdir;
      /// Sin(line_direction+PI/2)
      double sintic;
      /// Cos(line_direction+PI/2)      
      double costic;

      /*
       * This stuff is related to the data.
       */
      /// For the axis, this is the data's maximum for the axis to show.
      double max;
      /// For the axis, this is the data's maximum from which to draw tics.
      /// This is not necessarily within the bounds if the plot is tight.
      double axesMax;


      /// For the axis, this is the data's minimum for the axis to show.      
      double min;
      /// For the axis, this is the data's minimum from which to draw tics.
      /// This is not necessarily within the bounds if the plot is tight.
      double axesMin;


      /// For the axis, this is the gap between tick marks.  This is 0 if 
      /// this is to be determined by the Axis.
      double gap;
  };
}

#endif //VPLOT_AXIS_H

