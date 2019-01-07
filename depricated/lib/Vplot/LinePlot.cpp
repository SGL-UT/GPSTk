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

/// @file LinePlot.cpp Used to draw a line plot. Class definitions.

#include "LinePlot.hpp"

using namespace std;
using namespace vdraw;

namespace vplot
{
  void LinePlot::init(unsigned int issidx)
  {
    Color col [] = {Color::BLUE, Color::ORANGE, Color::GREEN, Color::DARK_PURPLE, 
      Color::YELLOW, Color::PINK, Color::NAVY, Color::KHAKI, Color::CARDINAL, 
      Color::CYAN, Color::MAGENTA, Color::BURNT_ORANGE, Color::CHARTREUSE, 
      Color::FOREST_GREEN, Color::SKY_BLUE, Color::BROWN, Color::VIOLET, 
      Color::GRAY, Color::TURQUOISE, Color::MAROON};

    for (unsigned int c=0; c<sizeof(col)/sizeof(col[0]); c++)  
      cvec.push_back(col[c]);

    StrokeStyle::dashLengthList solid, ldash, sdash;
    dvec.push_back(solid);
    ldash.push_back(4);
    ldash.push_back(2);
    dvec.push_back(ldash);
    sdash.push_back(2);
    sdash.push_back(2);
    dvec.push_back(sdash);

    // Initial index
    this->issidx = issidx % (cvec.size()*dvec.size());
    ssidx = issidx;
  }

  StrokeStyle LinePlot::pickNextSS(void)
  {
    if(ssidx == (cvec.size()*dvec.size()))
      ssidx = 0;

  //  if(ssidx == issidx)
	//; //Throw an error...doing same stuff again

    return pickNextSS(ssidx++);
  }

  StrokeStyle LinePlot::pickNextSS(int idx)
  {
    int cid = idx % cvec.size();
    int did = idx / cvec.size();

    //throw an exception for did>dlist.size()
    //if possible try (ssidx/clist.size())%dlist.size()
    // FIXME See above problem.  Maybe allow manual dlist add.

    return StrokeStyle(cvec[cid], .75, dvec[did]);
  }

  void LinePlot::draw(Frame *frame, int dir)
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

  void LinePlot::drawPlot(Frame* frame)
  {
    Frame innerFrame = getPlotArea(frame);

    // Obtain mins and maxes from the data
    double minXdata, maxXdata, minYdata, maxYdata;
    sl.findMinMax(minXdata,maxXdata,minYdata,maxYdata);

    // Assume these will be used as boundaries of the plots
    double minX=minXdata, maxX=maxXdata, minY=minYdata, maxY=maxYdata;
    
    // See if the user overrode the axes limits
    if (fixedXaxis)
    {
      minX = minx;
      maxX = minx + width;
    }

    if (fixedYaxis)
    {
       minY = miny;
       maxY = miny+height;
    }

    // Use default min/max
    sl.drawInFrame(innerFrame,minX,maxX,minY,maxY);

    setXAxis(minX, maxX);
    setYAxis(minY, maxY); 

    drawAxis(frame);
  }
}
