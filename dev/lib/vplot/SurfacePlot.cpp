#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/plot/SurfacePlot.cpp#10 $"

/// @file SurfacePlot.cpp Used to draw a surface plot. Class definitions.

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

#include "SurfacePlot.hpp"
#include "Rectangle.hpp"

using namespace std;
using namespace vdraw;

namespace vplot
{
  SurfacePlot::SurfacePlot(int iwidth, int iheight, const Palette& p)
    : Plot()
  {
    osr = osc = 1;
    boxes = false;

    width = iwidth;
    height = iheight;

    // Initialize the color array
    icm = InterpolatedColorMap(iwidth,iheight,p);

    axis_color.draw_labels = true;
    axis_color.tick_position = AxisStyle::BELOW;
    axis_color.label_position = AxisStyle::BELOW;    
    axis_color.label_style.setPointSize(8);
    axis_color.tight_bounds = true;
  }

  SurfacePlot::~SurfacePlot()
  {
  }

  void SurfacePlot::draw(Frame *frame, int dir)
  {
    HLayout hl(*frame,.9);
    Frame f = hl.getFrame(0);
    drawPlot(&f);
    f = hl.getFrame(1);
    BorderLayout bl(f,0,0,0,
        (ylabel.size()?label_style.getPointSize():0)+axis_bottom.label_style.getPointSize()+5);
    f = bl.getFrame(0);
    drawKey(&f); // ,dir);
  }

  void SurfacePlot::drawPlot(Frame* frame)
  {
    Frame innerFrame = getPlotArea(frame);

    // Draw Bitmap
    Bitmap b(innerFrame.lx(),innerFrame.ly(),innerFrame.ux(),innerFrame.uy(),icm);
    b.oversample(osr,osc);
    b.useBoxes(boxes);
    innerFrame << b;

    drawAxis(frame);
  }

  void SurfacePlot::drawKey(Frame *frame) //, int dir)
  {
    // Subframes
    BorderLayout bl(*frame,5); // Base on frame size, not points?
    Frame innerFrame = bl.getFrame(0);

    // TextStyle for label
    // Text label(colorlabel,label_style);
    if(colorlabel.size())
      *frame << Text(colorlabel.c_str(),label_style.getPointSize()+5,
          frame->getHeight()/2.0,label_style,Text::CENTER,90); 

    double xdist = (colorlabel.size()?label_style.getPointSize():0)+10;
    double bwidth = 20;

    Palette p = icm.getPalette();

    // TODO Add and validate direction  
    // Box
    ColorMap cm(p);
    Bitmap b(xdist,0,xdist+bwidth,innerFrame.getHeight(),cm);
    innerFrame << b
               << Rectangle(xdist,0,xdist+bwidth,innerFrame.getHeight());
    
    // Axis
    Axis a(xdist+bwidth,0,innerFrame.getHeight(),Axis::NORTH,p.getMin(),p.getMax()); // min max
    a.axis_style = axis_color;
    a.drawToFrame(innerFrame); 
  }
}
