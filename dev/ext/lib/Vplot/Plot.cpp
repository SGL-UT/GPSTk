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

/// @file Plot.cpp Basic plot information. Class definitions.

#include "Plot.hpp"

using namespace std;
using namespace vdraw;

namespace vplot
{
  Plot::Plot() 
  {
    minx = miny = 0;
    xgap = ygap = 0;
    width = height = 0;

    fixedXaxis = fixedYaxis = false;

    label_style = TextStyle();

    axis_top = AxisStyle();
    axis_bottom = AxisStyle();
    axis_left = AxisStyle();
    axis_right = AxisStyle();

    /// Generic defaults: Axis on each side, left and lower ones have labels
    axis_bottom.label_style.setPointSize(8);
    axis_bottom.tick_recursion_depth = 2;
    axis_bottom.tick_position = AxisStyle::ABOVE;
    axis_bottom.label_position = AxisStyle::BELOW;
    axis_bottom.tight_bounds = true;
    axis_top = axis_bottom;
    axis_top.draw_labels = false;
    axis_top.tick_position = AxisStyle::BELOW;
    axis_top.tight_bounds = true;

    axis_left.label_style.setPointSize(8);    
    axis_left.tick_recursion_depth = 3;
    axis_left.tick_position = AxisStyle::BELOW;
    axis_left.label_position = AxisStyle::ABOVE;
    axis_left.tight_bounds = true;    
    axis_right = axis_left;
    axis_right.draw_labels = false;
    axis_right.tick_position = AxisStyle::ABOVE;    
    axis_right.tight_bounds = true;
  }

  Frame Plot::getPlotArea(Frame* frame)
  {
    double left = (xlabel.size()?label_style.getPointSize():0)+40;
    double top = 5;
    double right = 5;
    double bottom = (ylabel.size()?label_style.getPointSize():0)+axis_bottom.label_style.getPointSize()+10;

    BorderLayout bl(*frame,left,top,right,bottom); // Base on frame size, not points?
    return(bl.getFrame(0));
  }
  void Plot::drawAxis(Frame* frame)
  {
    Frame innerFrame = getPlotArea(frame);

    const double left = innerFrame.actualX() - frame->actualX();
    const double bottom = innerFrame.actualY() - frame->actualY();
    //const double top = frame->getHeight() - bottom - innerFrame.getHeight();
    //const double right = frame->getWidth() - left - innerFrame.getWidth();

    // Draw axis
    Axis x(0,0,innerFrame.getWidth(),Axis::EAST,minx,minx+width);
    x.setGap(xgap);
    x.axis_style = axis_bottom;
    x.drawToFrame(innerFrame);
    x.setPosition(0,innerFrame.uy());
    x.axis_style = axis_top;        
    x.drawToFrame(innerFrame);

    Axis y(0,0,innerFrame.getHeight(),Axis::NORTH,miny,miny+height);
    y.setGap(ygap);    
    y.axis_style = axis_left;
    y.drawToFrame(innerFrame);
    y.setPosition(innerFrame.ux(),0);      
    y.axis_style = axis_right;
    y.drawToFrame(innerFrame);

    // Draw Labels
    if(xlabel.size())
      *frame << 
        Text(xlabel.c_str(),
          left+innerFrame.getWidth()/2,
          0,
          label_style,
          Text::CENTER);

    if(ylabel.size())
      *frame << 
        Text(
          ylabel.c_str(),
          label_style.getPointSize(),
          bottom+innerFrame.getHeight()/2,
          label_style,
          Text::CENTER,
          90); 
  }

}
