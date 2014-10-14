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

/// @file ScatterPlot.cpp Used to draw a scatter plot. Class definitions.

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

  //  if(midx == imidx)
	//;  //Throw an error...doing same stuff again

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
