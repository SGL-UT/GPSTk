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

/// @file Axis.cpp Used to draw an axis. Class definitions.

#include <cmath>
#include <cstdio>
#include "Axis.hpp"

namespace vplot
{
  const double Axis::RIGHT = 0;
  const double Axis::UP    = Axis::RIGHT+vdraw::HALF_PI;
  const double Axis::LEFT  = Axis::UP+vdraw::HALF_PI;
  const double Axis::DOWN  = Axis::LEFT+vdraw::HALF_PI;
  const double Axis::N     = Axis::UP;
  const double Axis::E     = Axis::RIGHT;
  const double Axis::S     = Axis::DOWN;
  const double Axis::W     = Axis::LEFT;
  const double Axis::NORTH = Axis::N;
  const double Axis::EAST  = Axis::E;
  const double Axis::SOUTH = Axis::S;
  const double Axis::WEST  = Axis::W;

  void Axis::init(double ix, double iy, double length, double direction, double imin, double imax, const AxisStyle &style)
  {
    setPosition(ix,iy);
    setLength(length);
    setAngle(direction);
    setRange(imin,imax);
    axis_style = style;
    gap=0.0;
    if(direction==NORTH)      axis_style.label_position=AxisStyle::ABOVE;
    else if(direction==EAST)  axis_style.label_position=AxisStyle::BELOW;
    else if(direction==SOUTH) axis_style.label_position=AxisStyle::BELOW;
    else if(direction==WEST)  axis_style.label_position=AxisStyle::ABOVE;
  }

  void Axis::setAngle(double angle)
  {
    // Save the angle, normalized to be between 0 and 360
    line_direction = fmod(angle,vdraw::TWO_PI);
    if(line_direction < 0) line_direction += 360.0;

    // Direction of the line, lets keep cos/sin to a minimum
    cosdir = cos(angle);
    sindir = sin(angle);

    // Ticks are 90 degrees rotated
    costic = cos(angle+vdraw::HALF_PI);
    sintic = sin(angle+vdraw::HALF_PI);
  }

  bool Axis::pointFromValue(double &x, double &y, double value)
  {
    if(value < min || value > max) return false;
    double ratio = (value-min)/(max-min);
    fromRatio(x,y,ratio);
    return true;
  }

  void Axis::drawToFrame(const vdraw::Frame &ff)
  {
    using namespace vdraw;

    // Set up the frame
    this->f = ff;

    // Draw the base line.
    double ex,ey;
    fromRatio(ex,ey,1.0);
    f.line(Line(x, y, ex, ey, axis_style.line_style));

    // Draw the tics and labels
    if(axis_style.logarithmic)
    {
      // TODO Draw logarithmic ticks/labels
    }
    else
    {
      double pos, val, distpos, distval;
      guessTickInfo(pos,val,distpos,distval);
      while(pos <= 1.0001)
      {
        // FIXME remove til next comment
        fromRatio(ex,ey,pos);
        // Draw necessary ticks
        axisTick(pos, distpos);
        // Draw label
        if(axis_style.draw_labels)
          label(val, axis_style.label_position, axis_style.label_style);
        // Increment
        pos += distpos;
        val += distval;
      }
      //std::cout << "fail pos:" << pos << std::endl;
    }

    // Forget the frame. Or remember for optional calls?
    //this->f = 0;
  }

  void Axis::axisTickRecursive(bool draw, int depth, double ratio, double dist, double length)
  {
    // Base case: 
    if(depth < 0) return;
    // Bounds checking
    if(ratio > 1.0001) return;
    if(ratio < 0.0)
    {
      if(ratio+dist < 0.0) return;
      draw = false;
    }
    // Draw
    if(draw)
    {
      drawTick(ratio,length);
    }
    // Recurse
    if(axis_style.subticks_per_tick < 1)
      return;
    double t = dist / (1.0+axis_style.subticks_per_tick);
    axisTickRecursive(false,depth-1,ratio       ,t,length*axis_style.tick_depth_multiplier);
    for(int i=1;i<=axis_style.subticks_per_tick;i++)
      axisTickRecursive(true ,depth-1,ratio+i*t,t,length*axis_style.tick_depth_multiplier);
  }

  void Axis::drawTick(double ratio, double length)
  {
    double x1,y1,x2,y2;
    fromTic(x1,y1,x2,y2,ratio,length);
    f.line(vdraw::Line(x1,y1,x2,y2,axis_style.tick_style));
  }

  void Axis::fromTic(double &x1, double &y1, double &x2, double &y2, 
      double distratio, double length)
  {
    double tx,ty;
    fromRatio(tx,ty,distratio);
    bool ta = axis_style.tick_position!=AxisStyle::BELOW;
    bool tb = axis_style.tick_position!=AxisStyle::ABOVE;
    x1 = tx; x2 = tx; y1 = ty; y2 = ty;
    if(ta&&tb) length=length/2;
    if(ta)
    {
      x1 = tx+costic*length;
      y1 = ty+sintic*length;
    }
    if(tb)
    {
      x2 = tx-costic*length;
      y2 = ty-sintic*length;
    }
  }

  void Axis::guessTickInfo(double &startpos, double &startval, double &distpos, double &distval)
  {
    double tickdx;

    if(gap<=0.0)
      tickdx = pow(10.0,floor(log10(fabs(max-min))));
    else
      tickdx = gap;
    
    if (fmod(max,tickdx)!= 0)
    {
      if (max>0)
        axesMax = max-fmod(max,tickdx)+tickdx;
      else
        axesMax = max-fmod(max,tickdx);
    }
    else
      axesMax = max;
    if (fmod(min,tickdx) != 0)
    {
      if (min<0)
        axesMin = min-fmod(min,tickdx)-tickdx;
      else
        axesMin = min-fmod(min,tickdx);
    }
    else
      axesMin = min;

    double w = max-min;    
    double aw = axesMax-axesMin;
    // See function header comments about these values.
    startpos = 0.0;
    startval = axesMin;
    distpos = tickdx/aw;
    distval = tickdx;
    //printf("--><%f,%f> (%f,%f) [%f,%f,%f,%f] %f\n",
    //  min,max,axesMin,axesMax,startpos,startval,distpos,distval,tickdx);
    if(axis_style.tight_bounds)
    {
      startpos = (axesMin-min)/w;
      distpos = tickdx/w;
    }
    else
    {
      max = axesMax;
      min = axesMin;
      w = aw;
    }
    if((w / tickdx) < 4 )
    {
      distpos /= 4;
      distval /= 4;
    }
  }

  bool Axis::label(double value, int direction, const vdraw::TextStyle &style)
  {
    using namespace vdraw;
    double x,y,rotation;
    if(!labelPoint(x,y,rotation,value,direction)) return false;
    char buffer [33];        
    int n=-1;
    if(axis_style.label_format.size() > 0)
    {
      n = std::sprintf(buffer,axis_style.label_format.c_str(),value);
      // This returns -1 if it fails, i.e. label_format was bad.
    }
    const char * c = NULL;
    if(n<0) // No label_format, or it failed
    {
      double pval = (value<0?-value:value);
      if(pval == 0)           c="0";
      else if(pval >= 100000) c="%4g"; 
      else if(pval >= 1)      c="%g";  //c="%4.f";
      else if(pval >= 0.1)    c="%g";  //c="%.1f";
      else if(pval >= 0.01)   c="%g";  //c="%.2f";
      else                    c="%4.e";  
      // FIXME Fix labels for values with small later numbers, for example if
      // value is like 1.3003003003 and want it displayed as 1.3
    }
    std::sprintf(buffer,c,value);
    std::string textString = std::string(buffer);
    Text t(textString.c_str(),x,y,style);
    return label(t,value,direction,style);
  }

  bool Axis::label(const char *str, double value, int direction, const vdraw::TextStyle &style)
  {
    using namespace vdraw;
    double x,y,rotation;
    if(!labelPoint(x,y,rotation,value,direction)) return false;
    Text t(str,x,y,style);
    return label(t,value,direction,style);
  }

  bool Axis::label(vdraw::Text &t, double value, int direction, const vdraw::TextStyle &style)
  {
    using namespace vdraw;
    // Quadrant
    //  2 | 1
    // ---|---
    //  3 | 4 
    short quad = 0;
    if(sindir>0)
    {
      if(cosdir>0) quad = 1;
      else         quad = 2;
    }
    else
    {
      if(cosdir>0) quad = 4;
      else         quad = 3;
    }
    bool above = direction==AxisStyle::ABOVE;
    
    if(line_direction == UP || line_direction == DOWN)
    {
      // Simply draw it to the left or right, horizontal
      // Set alignment
      if(cos(line_direction-(direction*vdraw::HALF_PI)) > 0) 
        t.setAlignment(Text::LEFT);
      else
        t.setAlignment(Text::RIGHT);
      // Bump down some based on text height
      t.setPosition(t.x,t.y-f.up()*style.getPointSize()/2); 
    }
    else if(line_direction == LEFT || line_direction == RIGHT)
    {
      // Simply draw it above or below, horizontal, centered
      // Set alignment
      t.setAlignment(Text::CENTER);
      // Bump down some based on text height
      if(sindir < 0.0 || (sindir == 0.0 && direction == AxisStyle::BELOW))
        t.setPosition(t.x,t.y-f.up()*style.getPointSize());
    }
    else
    {
      // Either make the text horizontal regardless of the line's directions or
      // angle the text to be 90 degrees off the direction of the line, in the
      // direction specified.  Make sure text is right-side-up. 
      // FIXME If above is made to mean "Above on the page" then...
      // if(cosdir<0) above = !above;
      if(false) // axis_style.label_horizontal
      {
        bool bumpdown = false;
        // Horizontal labels
        switch(quad)
        {
          case 1: 
            if(above) { t.setAlignment(Text::RIGHT); }
            else      { t.setAlignment(Text::LEFT); bumpdown=true; }
            break;
          case 2:
            if(above) { t.setAlignment(Text::RIGHT); bumpdown=true;}
            else      { t.setAlignment(Text::LEFT); }
            break;
          case 3:
            if(above) { t.setAlignment(Text::LEFT); bumpdown=true; }
            else      { t.setAlignment(Text::RIGHT); }
            break;
          case 4:
            if(above) { t.setAlignment(Text::LEFT); }
            else      { t.setAlignment(Text::RIGHT); bumpdown=true; }
            break;
        }
        if(bumpdown) t.setPosition(t.x,t.y-f.up()*style.getPointSize());
      }
      else
      {
        // TODO Angled labels
      }
    }
    f.text(t);
    return true;
  }

  bool Axis::labelPoint(double &x, double &y, double &rotation, double value, int direction)
  {
    if(!pointFromValue(x,y,value)) return false;
    double ticklen = axis_style.major_tick_length;
    if(axis_style.tick_position == AxisStyle::CENTER) ticklen/=2;
    // Adjust these as necessary... to get point away from the ticks themselves.
    if((direction==AxisStyle::ABOVE && axis_style.tick_position==AxisStyle::BELOW) ||
      (direction==AxisStyle::BELOW && axis_style.tick_position==AxisStyle::ABOVE))
      ticklen = 0.4*axis_style.label_style.getPointSize();
    else  
      ticklen += 3;
    x += (direction==AxisStyle::ABOVE?1:-1)*costic*ticklen*1.2; 
    y += f.up()*(direction==AxisStyle::ABOVE?1:-1)*sintic*ticklen*1.2;
    rotation = line_direction-vdraw::HALF_PI;
    // Determine optimal position based on which way the text will be 
    // Option for flat (horizontal on page) text vs angled?
    // TODO Angled things need changes?
    return true; 
  }

}

/* 
 * New (and improved!) notes on drawing ticks:
 * - Draw line
 * - Determine:
 *   * tick start position 
 *   * distance between ticks
 * - Recursively draw ticks
 *   * (Logarithmic version comes later)
 *
 * - Misc:
 *   * (?) Remember (or be able to recreate) tick positions
 *     in order to draw grid lines?  
 *     - This is most important with logarithmic plots
 *     - Potentially useful otherwise
 *     
 * - Grid notes:
 *   * Depth?
 *   * How long to draw them
 *   * StrokeStyle for each depth?  
 *     ~ Some dashed 
 *     ~ Some lines
 *     ~ Colors are important (blend with BG)
 *
 */
