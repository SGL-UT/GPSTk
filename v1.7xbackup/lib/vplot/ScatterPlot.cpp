#pragma ident "$Id$"

/// @file ScatterPlot.cpp Used to draw a scatter plot. Class definitions.

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

#include "ScatterPlot.hpp"

using namespace std;
using namespace vdraw;

namespace vplot
{
  void ScatterPlot::init(unsigned int imidx)
  {
    // Colors removed as they are considered a bit too light for markers:
    // Color::GREEN Color::PINK Color::CYAN Color::CHARTREUSE Color::TURQUOISE
    Color col [] = {Color::BLUE, Color::ORANGE, Color::DARK_PURPLE, 
      Color::YELLOW, Color::NAVY, Color::KHAKI, Color::CARDINAL, 
      Color::MAGENTA, Color::BURNT_ORANGE, 
      Color::FOREST_GREEN, Color::SKY_BLUE, Color::BROWN, Color::VIOLET, 
      Color::GRAY, Color::MAROON};

    for (unsigned int c=0; c<sizeof(col)/sizeof(col[0]); c++)  
      mcvec.push_back(col[c]);

    mvec.push_back(Marker::DOT);
    mvec.push_back(Marker::PLUS);
    mvec.push_back(Marker::X);    

    // Initial index
    this->imidx = imidx % (mcvec.size()*mvec.size());
    midx = imidx;
  }

  Marker ScatterPlot::pickNextMarker(void)
  {
    if(midx == (mcvec.size()*mvec.size()))
      midx = 0;

    if(midx == imidx); 
    //Throw an error...doing same stuff again

    return pickNextMarker(midx++);
  }

  Marker ScatterPlot::pickNextMarker(int idx)
  {
    int cid = idx % mcvec.size();
    int mid = idx / mcvec.size();

    //throw an exception for did>dlist.size()
    //if possible try (midx/clist.size())%dlist.size()

    return Marker(mvec[mid], .75, mcvec[cid]);
  }

}
